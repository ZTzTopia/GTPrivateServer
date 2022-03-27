#include <memory>

#include "player.h"
#include "eventmanager.h"

#include "events/tank/set_icon_state.h"
#include "events/tank/state.h"
#include "events/tank/tile_change_request.h"

#include "events/text/enter_game.h"
#include "events/text/input.h"
#include "events/text/join_request.h"
#include "events/text/quit.h"
#include "events/text/quit_to_exit.h"
#include "events/text/refresh_item_data.h"
#include "events/text/requestedName.h"

#include "../database/database.h"

namespace player {
    Player::Player(server::Server *server, ENetPeer *peer)
        : EventManager()
        , m_server(server)
        , m_peer(peer)
        , m_user_id(0)
        , m_raw_name("")
        , m_mac("")
        , m_display_name("")
        , m_country("us")
        , m_current_world("")
        , m_net_id(0)
        , m_pos(CL_Vec2f{})
    {
        peer->data = reinterpret_cast<void *>(peer->connectID);

        // Memory leak or no? or it destroyed after Player is destroyed?
        events::set_icon_state{ this };
        events::state{ this };
        events::tile_change_request{ this };

        events::enter_game{ this };
        events::input{ this };
        events::join_request{ this };
        events::quit{ this };
        events::quit_to_exit{ this };
        events::refresh_item_data{ this };
        events::requestedName{ this };

        m_inventory = new Inventory{};
    }

    Player::~Player() {
        save();
    }

    size_t Player::save(size_t token, bool insert) const {
        try {
            sqlpp::mysql::connection *db = database::get_database()->get_connection();

            database::Guests guests{};
            if (insert) {
                auto id = (*db)(insert_into(guests).set(
                    guests.name = m_raw_name,
                    guests.mac = m_mac,
                    guests.token = -1,
                    guests.current_world = m_current_world
                ));
                return id;
            }
            else {
                (*db)(update(guests).set(
                    guests.token = -1,
                    guests.current_world = m_current_world
                ).where(guests.id == m_user_id));
                return 0;
            }
        }
        catch(const std::exception &e) {
            spdlog::error("Error inserting new guest: {}", e.what());
        }

        return 0;
    }

    bool Player::load_() {
        m_inventory->size = 16;
        m_inventory->items.insert_or_assign(18, 1);
        m_inventory->items.insert_or_assign(32, 1);

        try {
            sqlpp::mysql::connection *db = database::get_database()->get_connection();

            database::Guests guests{};
            for (const auto &row : (*db)(select(all_of(guests)).from(guests).where(guests.name == m_raw_name and guests.mac == m_mac))) {
                if (row._is_valid) {
                    m_user_id = row.id;
                    m_current_world = row.current_world;
                    return true;
                }
            }
        }
        catch(const std::exception &e) {
            spdlog::error("Error selecting new guest: {}", e.what());
        }

        return false;
    }

    void Player::send_inventory() {
        uint32_t mem_need = m_inventory->items.size() * 4 + 8;
        auto *data = new uint8_t[mem_need];
        data[mem_need] = 0;

        uint8_t w = 0x01;
        std::memcpy(data, &w, 1);
        uint32_t mem_pos = 1;

        std::memcpy(data + mem_pos, &m_inventory->size, 4);
        mem_pos += 4;

        uint16_t inventory_size = m_inventory->items.size();
        std::memcpy(data + mem_pos, &inventory_size, 2);
        mem_pos += 2;

        for (auto &item : m_inventory->items) {
            std::memcpy(data + mem_pos, &item.first, 2);
            mem_pos += 2;

            std::memcpy(data + mem_pos, &item.second, 1);
            mem_pos += 1;

            uint8_t flags = 0;
            std::memcpy(data + mem_pos, &flags, 1);
            mem_pos += 1;
        }

        player::GameUpdatePacket game_update_packet{};
        game_update_packet.packet_type = player::PACKET_SEND_INVENTORY_STATE;
        game_update_packet.net_id = -1;
        game_update_packet.flags |= 0x8;
        game_update_packet.data_extended_size = mem_pos;
        game_update_packet.data_extended = reinterpret_cast<uint32_t&>(data);
        send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, data);
    }

    void Player::process_generic_text_or_game_message(const std::string &text) {
        std::string first_text{ text.substr(0, text.find('|')) };
        if (first_text == "action") {
            std::string action_text{ text.substr(text.find('|') + 1, text.find('\n') - (text.find('|') + 1)) };
            if (!execute(action_text, text)) {
                spdlog::error("Unhandled action: {}", action_text.c_str());
            }
        }
        else {
            if (!execute(first_text, text)) {
                spdlog::error("Unhandled generic text: {}", first_text.c_str());
            }
        }
    }

    void Player::process_game_packet(GameUpdatePacket *game_update_packet) {
        if (!execute("gup_" + std::to_string(game_update_packet->packet_type), game_update_packet)) {
            spdlog::error("Unhandled game update packet type: {}", game_update_packet->packet_type);
        }
    }

    int Player::send_packet(eNetMessageType type, const std::string &data) {
        int ret = -1;
        if (m_peer) {
            ENetPacket *packet = enet_packet_create(nullptr, data.length() + 5, ENET_PACKET_FLAG_RELIABLE);
            *(eNetMessageType *)packet->data = type;
            std::memcpy(packet->data + sizeof(eNetMessageType), data.c_str(), data.length());

            ret = enet_peer_send(m_peer, 0, packet) != 0;
            if (ret) {
                enet_packet_destroy(packet);
            }
        }

        return ret;
    }

    int Player::send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length, uint8_t *extended_data, enet_uint32 flags) {
        int ret = -1;
        if (m_peer) {
            if (length > 0xF4240) {
                return ret;
            }

            if (type == NET_MESSAGE_GAME_PACKET && (game_update_packet->flags & 0x8) != 0) {
                ENetPacket *packet = enet_packet_create(nullptr, length + game_update_packet->data_extended_size + 5, flags);
                *(eNetMessageType *)packet->data = type;
                std::memcpy(packet->data + sizeof(eNetMessageType), game_update_packet, length);
                std::memcpy(packet->data + length + sizeof(eNetMessageType), extended_data, game_update_packet->data_extended_size);

                ret = enet_peer_send(m_peer, 0, packet) != 0;
                if (ret) {
                    enet_packet_destroy(packet);
                }
            }
            else {
                ENetPacket *packet = enet_packet_create(nullptr, length + 5, flags);
                *(eNetMessageType *) packet->data = type;
                std::memcpy(packet->data + sizeof(eNetMessageType), game_update_packet, length);

                ret = enet_peer_send(m_peer, 0, packet) != 0;
                if (ret) {
                    enet_packet_destroy(packet);
                }
            }
        }

        return ret;
    }

    int Player::send_variant(VariantList &&variant_list, uint32_t net_id, enet_uint32 flags) {
        if (variant_list.Get(0).GetType() == eVariantType::TYPE_UNUSED) {
            return -1;
        }

        uint32_t data_size;
        uint8_t *data = variant_list.SerializeToMem(&data_size, nullptr);

        GameUpdatePacket game_update_packet{};
        game_update_packet.packet_type = PACKET_CALL_FUNCTION;
        game_update_packet.net_id = net_id;
        game_update_packet.flags |= 0x8;
        game_update_packet.data_extended_size = data_size;
        game_update_packet.data_extended = reinterpret_cast<uint32_t&>(data);

        int ret{ send_raw_packet(NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(GameUpdatePacket) - 4, data, flags) };

        delete data;
        return ret;
    }

    int Player::send_variant(VariantList &&variant_list, GameUpdatePacket *game_update_packet, enet_uint32 flags) {
        if (variant_list.Get(0).GetType() == eVariantType::TYPE_UNUSED) {
            return -1;
        }

        uint32_t data_size;
        uint8_t *data = variant_list.SerializeToMem(&data_size, nullptr);

        game_update_packet->packet_type = PACKET_CALL_FUNCTION;
        game_update_packet->flags |= 0x8;
        game_update_packet->data_extended_size = data_size;
        game_update_packet->data_extended = reinterpret_cast<uint32_t&>(data);

        int ret{ send_raw_packet(NET_MESSAGE_GAME_PACKET, game_update_packet, sizeof(GameUpdatePacket) - 4, data, flags) };

        delete data;
        return ret;
    }

    int Player::send_log(const std::string &log, bool on_console_message) {
        if (!on_console_message) {
            return send_packet(NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|{}", log));
        }

        return send_variant({
            "OnConsoleMessage",
            log
        });
    }
}

#include "player.h"
#include "eventmanager.h"

namespace player {
    Player::Player(ENetPeer *peer)
        : EventTrigger()
        , m_peer(peer) {
        peer->data = reinterpret_cast<void *>(peer->connectID);

        EventManager::load(this);
    }

    void Player::process_generic_text_or_game_message(const std::string &text) {
        m_last_packet_text = text;

        std::string first_text{ text.substr(0, text.find('|')) };
        if (first_text == "action") {
            std::string action_text{ text.substr(text.find('|') + 1, text.find('\n') - (text.find('|') + 1)) };
            if (!trigger(action_text)) {
                spdlog::error("Unhandled action: {}", action_text.c_str());
            }
        }
        else {
            if (!trigger(first_text)) {
                spdlog::error("Unhandled generic text: {}", first_text.c_str());
            }
        }

        m_last_packet_text.clear();
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

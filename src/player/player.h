#pragma once
#include <string>
#include <enet/enet.h>

#include "packet.h"
#include "eventmanager.h"
#include "inventory.h"
#include "../../vendor/proton/shared/util/Variant.h"

namespace server {
    class Server;
}

namespace player {
    class Player : public EventManager {
    public:
        Player(server::Server *server, ENetPeer *peer);
        ~Player();

        size_t save(size_t token = -1, bool insert = false) const;
        bool load_();

        void send_inventory();

        void process_generic_text_or_game_message(const std::string &text);
        void process_game_packet(GameUpdatePacket *game_update_packet);

        int send_packet(eNetMessageType type, const std::string &data);
        int send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length = sizeof(GameUpdatePacket) - 4, uint8_t *extended_data = nullptr, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, uint32_t net_id = -1, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, GameUpdatePacket *game_update_packet, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_log(const std::string &log, bool on_console_message = false);

        [[nodiscard]] server::Server *get_server() const { return m_server; }
        [[nodiscard]] ENetPeer *get_peer() const { return m_peer; }

        void set_user_id(uint32_t user_id) { m_user_id = user_id; }
        [[nodiscard]] uint32_t get_user_id() const { return m_user_id; }

        void set_raw_name(const std::string &raw_name) { m_raw_name = raw_name; }
        [[nodiscard]] std::string get_raw_name() const { return m_raw_name; }

        void set_display_name(const std::string &display_name) { m_display_name = display_name; }
        [[nodiscard]] std::string get_display_name() const { return m_display_name; }

        void set_mac(const std::string &mac) { m_mac = mac; }
        [[nodiscard]] std::string get_mac() const { return m_mac; }

        void set_country(const std::string &country) { m_country = country; }
        [[nodiscard]] std::string get_country() const { return m_country; }

        void set_current_world(const std::string &world_name) { m_current_world = world_name; }
        [[nodiscard]] std::string get_current_world() const { return m_current_world; }

        void set_net_id(uint32_t net_id) { m_net_id = net_id; }
        [[nodiscard]] uint32_t get_net_id() const { return m_net_id; }

        void set_pos(float x, float y) { m_pos.x = x; m_pos.y = y; }
        void set_pos(CL_Vec2f pos) { m_pos = pos; }
        [[nodiscard]] CL_Vec2f get_pos() const { return m_pos; }

    private:
        server::Server *m_server;
        ENetPeer *m_peer;

        uint64_t m_user_id;
        std::string m_raw_name;
        std::string m_display_name;
        std::string m_mac;
        std::string m_country;
        std::string m_current_world;

        uint32_t m_net_id;
        CL_Vec2f m_pos;

        Inventory *m_inventory;
    };
}

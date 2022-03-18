#pragma once
#include <string>
#include <enet/enet.h>

#include "packet.h"
#include "eventmanager.h"
#include "../proton/shared/util/Variant.h"

namespace player {
    class Player : public EventManager {
    public:
        Player(int server_id, ENetPeer *peer);
        ~Player() = default;

        void process_generic_text_or_game_message(const std::string &text);
        void process_game_packet(GameUpdatePacket *game_update_packet);

        int send_packet(eNetMessageType type, const std::string &data);
        int send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length = sizeof(GameUpdatePacket) - 4, uint8_t *extended_data = nullptr, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, uint32_t net_id = -1, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, GameUpdatePacket *game_update_packet, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_log(const std::string &log, bool on_console_message = false);

        [[nodiscard]] int get_server_id() const { return m_server_id; }
        [[nodiscard]] ENetPeer *get_peer() const { return m_peer; }

    public:
        uint64_t m_user_id;
        std::string m_raw_name;
        std::string m_display_name;
        std::string m_country;
        std::string m_current_world;
        uint32_t m_net_id;

    private:
        int m_server_id;
        ENetPeer *m_peer;
    };
}

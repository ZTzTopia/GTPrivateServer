#pragma once
#include <string>
#include <enet/enet.h>

#include "packet.h"
#include "eventtrigger.h"
#include "../proton/shared/util/Variant.h"

namespace player {
    class Player : public EventTrigger {
    public:
        explicit Player(ENetPeer *peer);
        ~Player() = default;

        void process_generic_text_or_game_message(const std::string &text);

        int send_packet(eNetMessageType type, const std::string &data);
        int send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length, uint8_t *extended_data, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, uint32_t net_id = -1, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_log(const std::string &log, bool on_console_message = false);

        [[nodiscard]] ENetPeer *get_peer() const { return m_peer; }

        [[nodiscard]] std::string get_last_packet_text() const { return m_last_packet_text; }

        [[nodiscard]] size_t get_current_world_name_hash() const { return m_current_world_name_hash; }
        void set_current_world_name_hash(size_t current_world_name_hash) { m_current_world_name_hash = current_world_name_hash; }

    private:
        ENetPeer *m_peer;
        std::string m_last_packet_text;
        size_t m_current_world_name_hash;
    };
}

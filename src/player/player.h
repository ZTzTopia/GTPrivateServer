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

        [[nodiscard]] ENetPeer *get_peer() const { return m_peer; }

        void process_generic_text_or_game_message(const std::string &text);

        int send_packet(eNetMessageType type, const std::string &data);
        int send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length, uint8_t *extended_data, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
        int send_variant(VariantList &&variant_list, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);

    private:
        ENetPeer *m_peer;
        std::string m_last_packet_text;
    };
}

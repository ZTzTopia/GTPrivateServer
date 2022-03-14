#pragma once
#include <string>
#include <enet/enet.h>

#include "packet.h"
#include "eventtrigger.h"

namespace player {
    class Player : public EventTrigger {
    public:
        explicit Player(ENetPeer *peer);
        ~Player() = default;

        enet_uint32 get_connect_id() const;

        void process_generic_text(const std::string &text);

        int send_packet(eNetMessageType type, const std::string &data);
        int send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length, uint8_t *extended_data, enet_uint32 flags);

    private:
        ENetPeer *m_peer;
        std::string m_last_packet_text;
    };
}

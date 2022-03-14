#pragma once
#include <string>
#include <enet/enet.h>

#include "packet.h"
#include "eventtrigger.h"

namespace player {
    class Player : public EventTrigger {
    public:
        explicit Player(ENetPeer *peer);
        ~Player();

        enet_uint32 get_connect_id() const;

        void process_generic_text(const std::string &text);

        int send_packet(eNetMessageType type, const std::string &data);

    private:
        ENetPeer *m_peer;
        std::string m_last_packet_text;
    };
}

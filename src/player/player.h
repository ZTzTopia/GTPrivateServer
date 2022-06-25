#pragma once
#include <enet/enet.h>

#include "peer.h"

namespace player {
    class Player : public player::Peer {
    public:
        explicit Player(ENetPeer* peer);
        ~Player() = default;

        int send_log(const std::string &log, bool on_console_message = false);

    public:
        void set_net_id(uint32_t net_id) { m_net_id = net_id; }
        uint32_t get_net_id() const { return m_net_id; }

    private:
        uint32_t m_net_id;
    };
}

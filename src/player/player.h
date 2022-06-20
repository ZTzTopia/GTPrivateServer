#pragma once
#include <enet/enet.h>

#include "peer.h"

namespace player {
    class Player : public player::Peer {
    public:
        explicit Player(ENetPeer* peer);
        ~Player() = default;

        int send_log(const std::string &log, bool on_console_message = false);
    };
}

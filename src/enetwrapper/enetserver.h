#pragma once
#include <enet/enet.h>

namespace gtps {
    class ENetServer {
    public:
        ENetServer();
        ~ENetServer();

        static int one_time_init();

        int create_host(enet_uint16 port, size_t peer_count);

    private:
        ENetHost *m_host;
    };
}
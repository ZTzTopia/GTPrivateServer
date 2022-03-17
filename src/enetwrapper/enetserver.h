#pragma once
#include <thread>
#include <enet/enet.h>

namespace enetwrapper {
    class ENetServer {
        friend class ServerGateway;
        friend class Server;
    public:
        ENetServer();
        ~ENetServer();

        static int one_time_init();

        int create_host(enet_uint16 port, size_t peer_count);

        void start_service();
        void service_thread();

        virtual void on_connect(ENetPeer *peer) = 0;
        virtual void on_receive(ENetPeer *peer, ENetPacket *packet) = 0;
        virtual void on_disconnect(ENetPeer *peer) = 0;

    protected:
        ENetHost *m_host;
        std::thread m_service_thread;
        std::atomic<bool> m_running;
    };
}
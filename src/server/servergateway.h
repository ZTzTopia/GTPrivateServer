#pragma once
#include "loadbalancer.h"
#include "../enetwrapper/enetserver.h"
#include "../player/playerpool.h"

namespace server {
    class ServerGateway : public enetwrapper::ENetServer {
    public:
        ServerGateway(enet_uint16 port, size_t max_peer);
        ~ServerGateway();

        void on_connect(ENetPeer *peer) override;
        void on_receive(ENetPeer *peer, ENetPacket *packet) override;
        void on_disconnect(ENetPeer *peer) override;

    private:
        LoadBalancer *m_load_balancer;
        player::PlayerPool *m_player_pool;
    };
}
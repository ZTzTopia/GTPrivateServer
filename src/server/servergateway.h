#pragma once
#include "../enetwrapper/enetserver.h"

namespace server {
    class ServerGateway : public enetwrapper::ENetServer {
    public:
        ServerGateway(enet_uint16 port, size_t max_peer);
        ~ServerGateway() = default;

        void on_connect(ENetPeer *peer) override;
        void on_receive(ENetPeer *peer, ENetPacket *packet) override;
        void on_disconnect(ENetPeer *peer) override;

    private:
        enet_uint16 m_port;
        size_t m_max_peer;
        size_t m_peer_count;
    };
}

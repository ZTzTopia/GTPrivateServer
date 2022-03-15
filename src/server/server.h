#pragma once
#include <vector>
#include <enet/enet.h>

#include "../enetwrapper/enetserver.h"

namespace server {
    class Server : public enetwrapper::ENetServer {
    public:
        Server(enet_uint16 port, size_t max_peer);
        ~Server() = default;

        enet_uint16 get_port() const { return m_port; }
        size_t get_peer_count() const { return m_peer_count; }
        size_t get_max_peer() const { return m_max_peer; }

        void on_connect(ENetPeer *peer) override;
        void on_receive(ENetPeer *peer, ENetPacket *packet) override;
        void on_disconnect(ENetPeer *peer) override;

    private:
        std::vector<ENetPeer *> m_peers;
        enet_uint16 m_port;
        size_t m_max_peer;
        size_t m_peer_count;
    };
}
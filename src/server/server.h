#pragma once
#include <vector>
#include <enet/enet.h>

#include "../enetwrapper/enetserver.h"

namespace server {
    class Server : public enetwrapper::ENetServer {
    public:
        Server();
        ~Server() = default;

        size_t get_peer_count() const;
        bool is_peer_in_server(enet_uint32 connect_id);

        void on_connect(ENetPeer *peer);
        void on_receive(ENetPeer *peer, ENetPacket *packet);
        void on_disconnect(ENetPeer *peer);

    private:
        std::vector<ENetPeer *> m_peers;
        size_t m_peer_count;
    };
}
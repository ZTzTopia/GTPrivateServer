#pragma once
#include <vector>
#include <enet/enet.h>

#include "../enetwrapper/enetserver.h"
#include "../player/playerpool.h"
#include "../world/worldpool.h"

namespace server {
    class Server : public enetwrapper::ENetServer {
    public:
        Server(int server_id, enet_uint16 port, size_t max_peer);
        ~Server() = default;

        void on_update();

        void on_connect(ENetPeer *peer) override;
        void on_receive(ENetPeer *peer, ENetPacket *packet) override;
        void on_disconnect(ENetPeer *peer) override;

        player::PlayerPool *get_player_pool() { return m_player_pool; }
        world::WorldPool *get_world_pool() { return m_world_pool; }

        enet_uint16 get_port() { return m_host->address.port; }
        size_t get_peer_count() { return m_host->connectedPeers; }
        size_t get_max_peer() { return m_host->peerCount; }

    private:
        int m_server_id;
        player::PlayerPool *m_player_pool;
        world::WorldPool *m_world_pool;
    };
}
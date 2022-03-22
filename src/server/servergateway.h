#pragma once
#include "../enetwrapper/enetserver.h"
#include "../player/playerpool.h"

namespace server {
    class ServerGateway : public enetwrapper::ENetServer {
    public:
        ServerGateway();
        ~ServerGateway() = default;

        bool initialize(enet_uint16 port, size_t max_peer);

        void on_connect(ENetPeer *peer) override;
        void on_receive(ENetPeer *peer, ENetPacket *packet) override;
        void on_disconnect(ENetPeer *peer) override;

    private:
        player::PlayerPool *m_player_pool;
    };
}

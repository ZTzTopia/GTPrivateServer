#include <spdlog/spdlog.h>

#include "server.h"
#include "../player/playerpool.h"

namespace server {
    Server::Server(enet_uint16 port, size_t max_peer)
        : m_port(port)
        , m_max_peer(max_peer)
        , m_peer_count(0) {
        if (create_host(port, max_peer) != 0) {
            spdlog::error("Server with port {} failed to start.", port);
            delete this;
            return;
        }

        start_service();
    }

    void Server::on_connect(ENetPeer *peer) {
        spdlog::info("Client connected. (id: {})", peer->connectID);

        m_peer_count++;
        m_peers.emplace_back(peer);

        player::Player *player = player::get_player_pool()->new_player(peer);
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");
    }

    void Server::on_receive(ENetPeer *peer, ENetPacket *packet) {
        player::Player *player = player::get_player_pool()->get_player(peer->connectID);
        if (!player) {
            enet_peer_disconnect_now(peer, 0);
            return;
        }

        player::eNetMessageType type = player::get_message_type(packet);
        spdlog::info("Type {}", type);

        switch (type) {
            case player::NET_MESSAGE_GENERIC_TEXT:
            case player::NET_MESSAGE_GAME_MESSAGE:
                // spdlog::info("Text {}", player::get_text(packet));
                player->process_generic_text_or_game_message(std::string(player::get_text(packet)));
                break;
            case player::NET_MESSAGE_GAME_PACKET:
                spdlog::info("Packet type {}", player::get_struct(packet)->packet_type);
                // player->process_game_packet(player::get_struct(packet));
                break;
            default:
                spdlog::error("Unknown message type: {}", type);
                break;
        }
    }

    void Server::on_disconnect(ENetPeer *peer) {
        m_peer_count--;
        m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), peer), m_peers.end());

        if (!peer->data) {
            return;
        }

        // no warning k.
        struct wow {
            enet_uint32 connect_id;
        };

        auto connect_id = ((wow *)peer->data)->connect_id;
        delete (wow *)peer->data;
        spdlog::info("Client disconnected. (id: {})", connect_id);

        peer->data = nullptr;

        player::get_player_pool()->remove_player(connect_id);
    }
}
#include <spdlog/spdlog.h>

#include "server.h"
#include "../player/playerpool.h"

namespace server {
    Server::Server() : m_peer_count(0) {
        create_host(17256, 32);
        start_service();
    }

    size_t Server::get_peer_count() const {
        return m_peer_count;
    }

    bool Server::is_peer_in_server(enet_uint32 connect_id) {
        auto found = std::find_if(m_peers.begin(), m_peers.end(), [connect_id](const auto &peer_found) {
            return peer_found->connectID == connect_id;
        });

        return found != m_peers.end();
    }

    void Server::on_connect(ENetPeer *peer) {
        spdlog::info("Client connected. (id: {})", peer->connectID);

        m_peer_count++;
        m_peers.emplace_back(peer);

        player::get_player_pool()->new_player(peer);

        player::Player *player = player::get_player_pool()->get_player(peer->connectID);
        if (!player) {
            spdlog::error("Player not found. wtf?");
            enet_peer_disconnect_later(peer, 0);
            return;
        }

        if (player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "") != 0) {
            spdlog::error("Failed to send hello message packet to client. wtf?");
            enet_peer_disconnect_later(peer, 0);
            return;
        }
    }

    void Server::on_receive(ENetPeer *peer, ENetPacket *packet) {
        player::Player *player = player::get_player_pool()->get_player(peer->connectID);
        if (!player) {
            spdlog::error("Player not found. wtf?");
            enet_peer_disconnect_later(peer, 0);
            return;
        }

        player::eNetMessageType type = player::get_message_type(packet);
        spdlog::info("Type {}", type);
        switch (type) {
            case player::NET_MESSAGE_GENERIC_TEXT:
                player->process_generic_text(std::string(player::get_text(packet)));
                break;
            case player::NET_MESSAGE_GAME_MESSAGE:
                // player->process_game_message(player::get_text(packet));
                break;
            case player::NET_MESSAGE_GAME_PACKET:
                // player->process_game_packet(player::get_struct(packet));
                break;
            default:
                spdlog::error("Unknown message type: {}", type);
                break;
        }
        // spdlog::info("Text {}", player::get_text(packet));
    }

    void Server::on_disconnect(ENetPeer *peer) {
        m_peer_count--;
        m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), peer), m_peers.end());
    }
}
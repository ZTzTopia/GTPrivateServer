#include <spdlog/spdlog.h>

#include "server.h"

namespace server {
    Server::Server(int server_id, enet_uint16 port, size_t max_peer) : m_server_id(server_id) {
        if (create_host(port, max_peer) != 0) {
            spdlog::error("Server with port {} failed to start.", port);
            delete this;
            return;
        }

        start_service();

        m_player_pool = new player::PlayerPool{};
        m_world_pool = new world::WorldPool{};
    }

    void Server::on_update() {
        for (ENetPeer *current_peer = m_host->peers; current_peer < &m_host->peers[m_host->peerCount]; ++current_peer) {
            if (current_peer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }

            // From github copilot 🤣. (good is work)
            float packet_loss = (float) current_peer->packetLoss / (float) ENET_PEER_PACKET_LOSS_SCALE;
            float packet_loss_variance = (float) current_peer->packetLossVariance / (float) ENET_PEER_PACKET_LOSS_SCALE;
            if (packet_loss > 0.0f && packet_loss_variance > 0.0f) {
                if (packet_loss_variance > packet_loss) {
                    packet_loss_variance = packet_loss;
                }

                float min = packet_loss - packet_loss_variance;
                float max = packet_loss + packet_loss_variance;

                if (min < 0.5f && max > 0.5f) {
                    /*spdlog::info("Client {} kicked for packet loss.", current_peer->connectID);
                    enet_peer_disconnect_later(current_peer, 0);
                    return;*/
                }
            }
        }
    }

    void Server::on_connect(ENetPeer *peer) {
        spdlog::info("Client connected. (id: {})", peer->connectID);

        player::Player *player = m_player_pool->new_player(m_server_id, peer);
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");
    }

    void Server::on_receive(ENetPeer *peer, ENetPacket *packet) {
        player::Player *player = m_player_pool->get_player(peer->connectID);
        if (!player) {
            enet_peer_disconnect_now(peer, 0);
            return;
        }

        if (packet->dataLength == 5) {
            return;
        }

        if (peer->packetsSent >= 256) {
            spdlog::info("Client {} kicked for flooding.", peer->connectID);
            enet_peer_disconnect_later(peer, 0);
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
                if (packet->dataLength < 5 + sizeof(player::GameUpdatePacket) - 4) {
                    return;
                }

                // spdlog::info("Packet type {}", player::get_struct(packet)->packet_type);
                player->process_game_packet(player::get_struct(packet));
                break;
            default:
                spdlog::error("Unknown message type: {}", type);
                break;
        }
    }

    void Server::on_disconnect(ENetPeer *peer) {
        if (!peer->data) {
            return;
        }

        auto connect_id = reinterpret_cast<uint32_t&>(peer->data);
        m_player_pool->remove_player(connect_id);

        spdlog::info("Client disconnected. (id: {})", connect_id);
    }
}
#include <spdlog/spdlog.h>

#include "servergateway.h"
#include "loadbalancing.h"
#include "serverpool.h"
#include "../config.h"
#include "../utils/textparse.h"

namespace server {
    ServerGateway::ServerGateway(enet_uint16 port, size_t max_peer) {
        if (create_host(port, max_peer) != 0) {
            spdlog::error("Server with port {} failed to start.", port);
            delete this;
            return;
        }

        start_service();

        m_player_pool = new player::PlayerPool{};
    }

    void ServerGateway::on_connect(ENetPeer *peer) {
        spdlog::info("Client connected to server gateway. (id: {})", peer->connectID);

        player::Player *player = m_player_pool->new_player(-1, peer);
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");
    }

    void ServerGateway::on_receive(ENetPeer *peer, ENetPacket *packet) {
        player::Player *player = m_player_pool->get_player(peer->connectID);
        if (!player) {
            enet_peer_disconnect_later(peer, 0);
            return;
        }

        // This is send after OnSendToServer.
        if (player::get_message_type(packet) == player::NET_MESSAGE_GAME_PACKET) {
            if (player::get_struct(packet)->packet_type == player::PACKET_DISCONNECT) {
                auto connect_id = reinterpret_cast<uint32_t&>(peer->data);
                m_player_pool->remove_player(connect_id);

                enet_peer_disconnect_now(peer, 0);

                spdlog::info("Client disconnected from server gateway. (id: {})", connect_id);
                return;
            }
        }

        if (m_host->connectedPeers >= m_host->peerCount - static_cast<size_t>(static_cast<float>(m_host->peerCount) / 1.2)) {
            player->send_log("`4OOPS: ``Too many people logging in at once. Please click `5CANCEL ``and try again in a few seconds.");
            enet_peer_disconnect_later(peer, 0);
            return;
        }

        if (get_server_pool()->get_servers().empty()) {
            player->send_log("Server is currently initializing or re-syncing with sub-server. Please try again in a minute.");
            return;
        }

        static LoadBalancing load_balancing{};
        size_t total_count_peer = 0;

        for (auto &server: get_server_pool()->get_servers()) {
            total_count_peer += server->get_peer_count();
            if (server->get_peer_count() >= server->get_max_peer()) {
                load_balancing.remove_server(server);
                continue;
            }

            load_balancing.add_server(server);
        }

        Server *best_server = load_balancing.get_server_with_minimum_player();
        if (best_server) {
            TextParse text_parse{ player::get_text(packet) };
            size_t user_hash{ std::hash<std::string>{}(text_parse.get("requestedName", 1)) };
            user_hash += best_server->get_port();

            player->send_variant({
                "OnSendToServer",
                best_server->get_port(),
                -1,
                static_cast<int32_t>(user_hash),
                fmt::format("{}|", config::address),
                1
            });

            return;
        }

        player->send_log(fmt::format("`4 SERVER OVERLOADED : ``Sorry, our servers are currently at max capacity with {} online, please try again later. We are working to improve this!", total_count_peer));
        enet_peer_disconnect_later(peer, 0);
    }

    void ServerGateway::on_disconnect(ENetPeer *peer) {}
}
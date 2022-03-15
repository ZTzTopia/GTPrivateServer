#include <spdlog/spdlog.h>

#include "servergateway.h"
#include "serverpool.h"
#include "../player/playerpool.h"

namespace server {
    ServerGateway::ServerGateway(enet_uint16 port, size_t max_peer)
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

    void ServerGateway::on_connect(ENetPeer *peer) {
        spdlog::info("Client connected to server gateway. (id: {})", peer->connectID);

        player::Player *player = player::get_player_pool()->new_player(peer);
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");

        m_peer_count++;
    }

    void ServerGateway::on_receive(ENetPeer *peer, ENetPacket *packet) {
        player::Player *player = player::get_player_pool()->get_player(peer->connectID);
        if (!player) {
            enet_peer_disconnect_now(peer, 0);
            return;
        }

        if (m_peer_count >= m_max_peer - static_cast<size_t>(static_cast<float>(m_max_peer) / 1.2)) {
            player->send_packet(player::NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|`4OOPS: ``Too many people logging in at once. Please click `5CANCEL ``and try again in a few seconds."));
            enet_peer_disconnect_later(peer, 0);
            return;
        }

        size_t peer_count = 0;
        if (!get_server_pool()->get_servers().empty()) {
            for (auto &server: get_server_pool()->get_servers()) {
                if (server->get_peer_count() >= server->get_max_peer()) {
                    peer_count += server->get_peer_count();
                    continue;
                }

                VariantList variant_list{};
                variant_list.Get(0).Set("OnSendToServer");
                variant_list.Get(1).Set(server->get_port());
                variant_list.Get(2).Set("-1");
                variant_list.Get(3).Set("1233");
                variant_list.Get(4).Set("127.0.0.1|");
                variant_list.Get(5).Set("1");

                uint32_t data_size;
                uint8_t *data = variant_list.SerializeToMem(&data_size, nullptr);

                variant_list.SerializeFromMem(data, data_size, nullptr);
                printf("%s\n", variant_list.GetContentsAsDebugString().c_str());

                player::GameUpdatePacket packet_{};
                packet_.packet_type = player::PACKET_CALL_FUNCTION;
                packet_.net_id = -1;
                packet_.flags |= 0x8;
                packet_.data_extended_size = data_size;
                player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &packet_, sizeof(player::GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

                delete[] data;

                packet_ = {};
                packet_.packet_type = player::PACKET_DISCONNECT;
                player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &packet_, sizeof(player::GameUpdatePacket) - 4, nullptr, ENET_PACKET_FLAG_RELIABLE);

                enet_peer_disconnect_later(peer, 0);
                return;
            }

            player->send_packet(player::NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|`4 SERVER OVERLOADED : ``Sorry, our servers are currently at max capacity with {} online, please try again later. We are working to improve this!", peer_count));
            enet_peer_disconnect_later(peer, 0);
            return;
        }
    }

    void ServerGateway::on_disconnect(ENetPeer *peer) {
        m_peer_count--;

        if (!peer->data) {
            return;
        }

        // no warning k.
        struct wow {
            enet_uint32 connect_id;
        };

        auto connect_id = ((wow *)peer->data)->connect_id;
        spdlog::info("Client disconnected from server gateway. (id: {})", connect_id);

        player::get_player_pool()->remove_player(connect_id);

        delete (wow *)peer->data;
        peer->data = nullptr;
    }
}
#include <spdlog/spdlog.h>

#include "server.h"
#include "../enetwrapper/enet_wrapper.h"

namespace server {
    Server::Server(std::shared_ptr<Config> config) : m_config{ std::move(config) }
    {
        m_http = std::make_unique<Http>(config);
        m_event_pool = std::make_unique<event::EventPool>();
        m_player_pool = std::make_unique<player::PlayerPool>();
    }

    bool Server::start()
    {
        if (!m_http->listen("0.0.0.0", 443)) {
            spdlog::error("Failed to listen on port 443");
            return false;
        }

        if (!enetwrapper::ENetWrapper::one_time_init()) {
            spdlog::error("Failed to initialize ENet server.");
            return false;
        }

        if (!create_host(16999, 1)) {
            spdlog::error("Failed to create ENet host.");
            return false;
        }

        start_service();
        return true;
    }

    void Server::on_connect(ENetPeer* peer)
    {
        spdlog::info("New client connected to server! -> ({}:{} - {})", peer->address.host, peer->address.port, peer->connectID);

        std::shared_ptr<player::Player> player{ m_player_pool->new_player(peer) };
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");
    }

    void Server::on_receive(ENetPeer* peer, ENetPacket* packet)
    {
        std::shared_ptr<player::Player> player = m_player_pool->get_player(peer->connectID);
        if (!player) {
            enet_peer_disconnect_now(peer, 0);
            return;
        }

        player::eNetMessageType type{ player::message_type_to_enum(packet) };
        switch (type) {
            case player::NET_MESSAGE_GENERIC_TEXT:
            case player::NET_MESSAGE_GAME_MESSAGE: {
                event::EventContext ctx{};
                m_event_pool->try_find_and_fire_event(player::get_text(packet), ctx);
                break;
            }
            case player::NET_MESSAGE_GAME_PACKET: {
                event::EventContext ctx{};
                m_event_pool->try_find_and_fire_event(std::to_string(player::get_struct(packet)->type), ctx);
                break;
            }
            default:
                spdlog::warn("Unknown message type: {}", type);
                break;
        }
    }

    void Server::on_disconnect(ENetPeer* peer)
    {
        if (!peer->data) {
            return;
        }

        uint32_t connect_id;
        std::memcpy(&connect_id, peer->data, sizeof(uint32_t));
        std::free(peer->data);

        spdlog::info("Client disconnected from server! -> ({}:{} - {})", peer->address.host, peer->address.port, connect_id);

        m_player_pool->remove_player(connect_id);
    }
}

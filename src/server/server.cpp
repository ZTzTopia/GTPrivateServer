#include <spdlog/spdlog.h>

#include "server.h"
#include "../enetwrapper/enet_wrapper.h"

namespace server {
    Server::Server(std::shared_ptr<Config> config) : m_config{ std::move(config) }, m_login_per_second{ 0 }
    {
        m_http = std::make_unique<Http>(config);
        m_item_db = std::make_shared<item::ItemDB>();
        m_event_pool = std::make_shared<event::EventPool>();
        m_player_pool = std::make_shared<player::PlayerPool>();
        m_world_pool = std::make_shared<world::WorldPool>();
    }

    bool Server::start()
    {
        if (!m_http->listen("0.0.0.0", 443)) {
            spdlog::error("Failed to listen on port 443.");
            return false;
        }

        if (!m_item_db->load()) {
            spdlog::error("Failed to load item database.");
            return false;
        }

        if (!enetwrapper::ENetWrapper::one_time_init()) {
            spdlog::error("Failed to initialize ENet server.");
            return false;
        }

        if (!create_host(16999, 36 /* config max players + 4 */)) {
            spdlog::error("Failed to create ENet host.");
            return false;
        }

        start_service();
        return true;
    }

    void Server::update_last_login()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now{ std::chrono::high_resolution_clock::now() };
        if (now - m_last_login_time > std::chrono::seconds(1)) {
            m_login_per_second = 0;
        }

        m_last_login_time = std::chrono::high_resolution_clock::now();
        m_login_per_second++;
    }

    void Server::on_connect(ENetPeer* peer)
    {
        spdlog::info("New client connected to server! -> ({}:{} - {})", peer->address.host, peer->address.port, peer->connectID);

        update_last_login();

        std::shared_ptr<player::Player> player{ m_player_pool->new_player(peer) };
        player->send_packet(player::NET_MESSAGE_SERVER_HELLO, "");
    }

    void Server::on_receive(ENetPeer* peer, ENetPacket* packet)
    {
        std::shared_ptr<player::Player> player = m_player_pool->get_player(peer->connectID);
        if (!player) {
            player->send_log("Server requested you to re-login.");
            player->disconnect_later();
            return;
        }

        if (m_player_pool->get_player_count() > 32 /* config max players */) {
            player->send_log("`4 SERVER OVERLOADED : ``Sorry, our servers are currently at max capacity with 32 online, please try again later. We are working to improve this!");
            player->disconnect_later();
            return;
        }

        if (m_login_per_second > 1 /* config max login per second */) {
            player->send_log("`4OOPS: ``Too many people logging in at once. Please click `5CANCEL ``and try again in a few seconds.");
            player->disconnect_later();
            return;
        }

        player::eNetMessageType type{ player::message_type_to_enum(packet) };
        switch (type) {
            case player::NET_MESSAGE_GENERIC_TEXT:
            case player::NET_MESSAGE_GAME_MESSAGE: {
                event::EventContext ctx{};
                ctx.text = player::get_text(packet);
                ctx.player = player;
                ctx.item_db = m_item_db;
                ctx.world_pool = m_world_pool;

                m_event_pool->try_find_and_fire_event(ctx.text, ctx);
                break;
            }
            case player::NET_MESSAGE_GAME_PACKET: {
                event::EventContext ctx{};
                ctx.tank = player::get_struct(packet);
                ctx.player = player;
                ctx.item_db = m_item_db;
                ctx.world_pool = m_world_pool;

                m_event_pool->try_find_and_fire_event(std::to_string(ctx.tank->type), ctx);
                break;
            }
            default:
                spdlog::warn("Unknown message type: {}.", type);
                break;
        }
    }

    void Server::on_disconnect(ENetPeer* peer)
    {
        if (!peer->data) {
            spdlog::info("Client disconnected from server! -> ({}:{} - null)", peer->address.host, peer->address.port);
            return;
        }

        uint32_t connect_id;
        std::memcpy(&connect_id, peer->data, sizeof(uint32_t));
        std::free(peer->data);

        spdlog::info("Client disconnected from server! -> ({}:{} - {})", peer->address.host, peer->address.port, connect_id);

        m_player_pool->remove_player(connect_id);
    }
}

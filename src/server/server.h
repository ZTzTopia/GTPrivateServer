#pragma once
#include "http.h"
#include "../config.h"
#include "../enetwrapper/enet_server.h"
#include "../event/event_pool.h"
#include "../item/itemdb.h"
#include "../player/player_pool.h"

namespace server {
    class Server : public enetwrapper::ENetServer {
    public:
        explicit Server(std::shared_ptr<Config> config);
        ~Server() = default;

        bool start();

        void on_connect(ENetPeer* peer) override;
        void on_receive(ENetPeer* peer, ENetPacket* packet) override;
        void on_disconnect(ENetPeer* peer) override;

    private:
        std::shared_ptr<Config> m_config;
        std::shared_ptr<Http> m_http;
        std::shared_ptr<item::ItemDB> m_item_db;
        std::shared_ptr<event::EventPool> m_event_pool;
        std::shared_ptr<player::PlayerPool> m_player_pool;
    };
}

#include <spdlog/spdlog.h>

#include "server.h"
#include "../enetwrapper/enet_wrapper.h"

namespace server {
    Server::Server(std::shared_ptr<Config> config) : m_config{ std::move(config) }
    {
        m_http = std::make_unique<Http>(config);
        m_http->listen("0.0.0.0", 443);
    }

    bool Server::start()
    {
        if (!enetwrapper::ENetWrapper::one_time_init()) {
            spdlog::error("Failed to initialize ENet server.");
            return false;
        }

        if (!create_host(16999, 1)) {
            return false;
        }

        start_service();
        return true;
    }

    void Server::on_connect(ENetPeer* peer)
    {
        spdlog::info("New client connected to proxy server!");
    }

    void Server::on_receive(ENetPeer* peer, ENetPacket* packet)
    {
        spdlog::info("Received packet from client!");
    }

    void Server::on_disconnect(ENetPeer* peer)
    {
        spdlog::info("Client disconnected from proxy server!");
    }
}

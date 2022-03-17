#include "serverpool.h"

namespace server {
    ServerPool::ServerPool() {
        m_running.store(false);
    }

    ServerPool::~ServerPool() {
        if (m_running.load()) {
            m_running.store(false);
            m_pool_thread.join();
        }

        for (auto &server : m_servers) {
            delete server;
        }

        m_servers.clear();
    }

    void ServerPool::start() {
        if (m_running.load()) {
            return;
        }

        m_running.store(true);
        std::thread thread{ &ServerPool::pool_thread, this };
        m_pool_thread = std::move(thread);
    }

    void ServerPool::pool_thread() {
        while (m_running.load()) {
            for (auto &server: m_servers) {
                server->on_update();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void ServerPool::new_server(enet_uint16 port, size_t max_peer) {
        auto *server = new Server{
            port,
            max_peer
        };
        add_server(server);
    }

    void ServerPool::add_server(Server *server) {
        m_servers.emplace_back(server);
    }

    void ServerPool::remove_server(Server *server) {
        if (!server) {
            return;
        }

        delete server;
        m_servers.erase(std::remove(m_servers.begin(), m_servers.end(), server), m_servers.end());
        m_servers.shrink_to_fit();
    }

    void ServerPool::remove_server(int index) {
        if (index < 0 || index >= m_servers.size()) {
            return;
        }

        delete m_servers.at(index);
        m_servers.erase(m_servers.begin() + index);
        m_servers.shrink_to_fit();
    }

    Server *ServerPool::get_server(int index) {
        if (index < 0 || index >= m_servers.size()) {
            return nullptr;
        }

        return m_servers.at(index);
    }

    std::vector<Server *> ServerPool::get_servers() {
        return m_servers;
    }

    size_t ServerPool::get_server_count() {
        return m_servers.size();
    }
}
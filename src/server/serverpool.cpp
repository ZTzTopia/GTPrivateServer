#include "serverpool.h"

namespace server {
    ServerPool::~ServerPool() {
        for (auto &server : m_servers) {
            delete server;
        }

        m_servers.clear();
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
#include "serverpool.h"

namespace server {
    void ServerPool::new_server() {
        auto *server = new Server{};
        add_server(server);
    }

    void ServerPool::add_server(Server *server) {
        m_servers.emplace_back(server);
    }

    void ServerPool::remove_server(Server *server) {
        m_servers.erase(std::remove(m_servers.begin(), m_servers.end(), server), m_servers.end());
    }

    void ServerPool::remove_server(int index) {
        if (index < 0 || index >= m_servers.size()) {
            return;
        }

        m_servers.erase(m_servers.begin() + index);
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

    void ServerPool::on_connect(ENetPeer *peer) {
        for (auto it = m_servers.begin(); it != m_servers.end(); it++) {
            Server *server = *it;
            if (server->get_peer_count() < 32) {
                server->on_connect(peer);
            }
        }
    }

    void ServerPool::on_receive(ENetPeer *peer, ENetPacket *packet) {
        for (auto it = m_servers.begin(); it != m_servers.end(); it++) {
            Server *server = *it;
            if (peer->connectID != 0 && server->is_peer_in_server(peer->connectID)) {
                // TODO: We need to checking here if the packet enter world.
                server->on_receive(peer, packet);
            }
        }
    }

    void ServerPool::on_disconnect(ENetPeer *peer) {
        for (auto it = m_servers.begin(); it != m_servers.end(); it++) {
            Server *server = *it;
            if (peer->connectID != 0 && server->is_peer_in_server(peer->connectID)) {
                server->on_disconnect(peer);
            }
        }
    }
}
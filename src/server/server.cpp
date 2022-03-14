#include <spdlog/spdlog.h>

#include "server.h"

namespace server {
    Server::Server() : m_peer_count(0) {
        create_host(17256, 32);
        start_service();
    }

    size_t Server::get_peer_count() const {
        return m_peer_count;
    }

    bool Server::is_peer_in_server(ENetPeer *peer) {
        auto found = std::find_if(m_peers.begin(), m_peers.end(), [peer](const auto &peer_found) {
            return peer_found == peer;
        });

        return found != m_peers.end();
    }

    void Server::on_connect(ENetPeer *peer) {
        m_peer_count++;
        m_peers.emplace_back(peer);

        spdlog::info("Client connected: {}", peer->address.host);
        send_packet(peer, NET_MESSAGE_SERVER_HELLO, "1");
    }

    void Server::on_receive(ENetPeer *peer, ENetPacket *packet) {
        eNetMessageType type = get_message_type(packet);
        spdlog::info("Received message of type {}", static_cast<int>(type));
    }

    void Server::on_disconnect(ENetPeer *peer) {
        m_peer_count--;
        m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), peer), m_peers.end());
    }
}
#include "player.h"
#include "eventmanager.h"

namespace player {
    Player::Player(ENetPeer *peer)
        : EventTrigger()
        , m_peer(peer) {
        EventManager::load(this);
    }

    enet_uint32 Player::get_connect_id() const {
        return m_peer->connectID;
    }

    void Player::process_generic_text(const std::string &text) {
        m_last_packet_text = text;
        trigger(text.substr(0, text.find('|')));
        m_last_packet_text.clear();
    }

    int Player::send_packet(eNetMessageType type, const std::string &data) {
        int ret = -1;
        if (m_peer) {
            ENetPacket *packet = enet_packet_create(nullptr, data.length() + 5, ENET_PACKET_FLAG_RELIABLE);
            *(eNetMessageType *)packet->data = type;
            std::memcpy(packet->data + sizeof(eNetMessageType), data.c_str(), data.length());

            ret = enet_peer_send(m_peer, 0, packet) != 0;
            if (ret) {
                enet_packet_destroy(packet);
            }
        }

        return ret;
    }

    int Player::send_raw_packet(eNetMessageType type, GameUpdatePacket *game_update_packet, size_t length, uint8_t *extended_data, enet_uint32 flags) {
        int ret = -1;
        if (m_peer) {
            if (length > 0xF4240) {
                return ret;
            }

            ENetPacket* packet = enet_packet_create(nullptr, length + 5, flags);
            if (type == NET_MESSAGE_GAME_PACKET && (game_update_packet->flags & 8) != 0) {
                enet_packet_destroy(packet);
                packet = enet_packet_create(nullptr, length + game_update_packet->data_extended_size + 5, flags);
            }

            *(eNetMessageType*)packet->data = type;
            std::memcpy(packet->data + sizeof(eNetMessageType), game_update_packet, length);

            if (type == NET_MESSAGE_GAME_PACKET && (game_update_packet->flags & 8) != 0) {
                std::memcpy(packet->data + length + sizeof(eNetMessageType), extended_data, game_update_packet->data_extended_size);
            }

            ret = enet_peer_send(m_peer, 0, packet) != 0;
            if (ret) {
                enet_packet_destroy(packet);
            }
        }

        return ret;
    }
}

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
            ENetPacket *packet = enet_packet_create(nullptr, data.length() + 5, 1);
            *(eNetMessageType *)packet->data = type;
            std::memcpy(packet->data + sizeof(eNetMessageType), data.c_str(), data.length());

            ret = enet_peer_send(m_peer, 0, packet) != 0;
            if (ret) {
                enet_packet_destroy(packet);
            }
        }

        return ret;
    }
}

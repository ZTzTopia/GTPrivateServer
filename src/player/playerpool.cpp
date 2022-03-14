#include "playerpool.h"

namespace player {
    void PlayerPool::new_player(ENetPeer *peer) {
        auto *player = new Player{ peer };
        add_player(player);
    }

    void PlayerPool::add_player(Player *player) {
        m_players.emplace_back(player);
    }

    void PlayerPool::remove_player(Player *player) {
        m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
    }

    void PlayerPool::remove_player(enet_uint32 connect_id) {
        for (auto &player : m_players) {
            if (player->get_connect_id() == connect_id) {
                m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
                break;
            }
        }
    }

    Player *PlayerPool::get_player(enet_uint32 connect_id) {
        if (connect_id == 0) {
            return nullptr;
        }

        for (auto &player : m_players) {
            if (player->get_connect_id() == connect_id) {
                return player;
            }
        }

        return nullptr;
    }

    std::vector<Player *> PlayerPool::get_players() {
        return m_players;
    }

    size_t PlayerPool::get_player_count() {
        return m_players.size();
    }
}

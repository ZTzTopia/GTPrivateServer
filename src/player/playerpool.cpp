#include "playerpool.h"

namespace player {
    PlayerPool::~PlayerPool() {
        for (auto &player : m_players) {
            delete player;
        }

        m_players.clear();
    }

    Player *PlayerPool::new_player(ENetPeer *peer) {
        auto *player = new Player{ peer };
        add_player(player);
        return player;
    }

    void PlayerPool::add_player(Player *player) {
        m_players.emplace_back(player);
    }

    void PlayerPool::remove_player(Player *player) {
        delete player;
        m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
        m_players.shrink_to_fit();
    }

    void PlayerPool::remove_player(enet_uint32 connect_id) {
        for (auto &player : m_players) {
            if (player->get_peer()->connectID == connect_id) {
                delete player;
                m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
                m_players.shrink_to_fit();
                break;
            }
        }
    }

    Player *PlayerPool::get_player(enet_uint32 connect_id) {
        if (connect_id == 0) {
            return nullptr;
        }

        for (auto &player : m_players) {
            if (player->get_peer()->connectID == connect_id) {
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

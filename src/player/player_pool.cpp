#include "player_pool.h"

namespace player {
    PlayerPool::~PlayerPool()
    {
        for (auto& player : m_players) {
            player.second.reset();
        }

        m_players.clear();
    }

    std::shared_ptr<Player> PlayerPool::new_player(ENetPeer* peer)
    {
        auto player{ std::make_shared<Player>(peer) };
        m_players[peer->connectID] = player;
        return player;
    }

    void PlayerPool::remove_player(uint32_t connect_id)
    {
        m_players[connect_id].reset();
        m_players.erase(connect_id);
    }

    std::shared_ptr<Player> PlayerPool::get_player(uint32_t connect_id)
    {
        for (auto& player : m_players) {
            if (player.first == connect_id) {
                return player.second;
            }
        }

        return nullptr;
    }
}
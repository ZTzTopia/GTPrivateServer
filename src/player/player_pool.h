#pragma once
#include <memory>
#include <unordered_map>
#include <enet/enet.h>

#include "player.h"

namespace player {
    class PlayerPool {
    public:
        PlayerPool() = default;
        ~PlayerPool();

        std::shared_ptr<Player> new_player(ENetPeer* peer);
        void remove_player(uint32_t connect_id);

        std::shared_ptr<Player> get_player(uint32_t connect_id);

    public:
        std::size_t get_player_count() const { return m_players.size(); }

    private:
        std::unordered_map<uint32_t, std::shared_ptr<Player>> m_players;
    };
}
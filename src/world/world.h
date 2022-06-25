#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "tile.h"
#include "../player/player.h"

namespace world {
    class World {
    public:
        explicit World(std::string name);
        ~World() = default;

        void generate();

        std::size_t calculate_memory_usage();
        std::vector<uint8_t> pack_to_memory();

        uint32_t add_player(const std::shared_ptr<player::Player>& player);

    private:
        uint16_t m_version;
        std::string m_name;
        std::vector<Tile*> m_tiles;

        uint32_t m_net_id;
        std::unordered_map<uint32_t, std::shared_ptr<player::Player>> m_players;
    };
}

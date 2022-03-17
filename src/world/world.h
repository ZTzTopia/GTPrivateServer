#pragma once
#include <vector>

#include "tile.h"
#include "../player/player.h"

namespace world {
    class World {
    public:
        World(const std::string &name) {};

        void generate_new_world(uint32_t width = 100, uint32_t height = 60);

        uint8_t *serialize_to_mem(uint32_t *size_out, uint8_t *dest);

        size_t get_world_name_hash() const { return m_world_name_hash; }

    private:
        size_t m_world_name_hash;
        std::vector<Tile *> m_tiles;
        std::vector<player::Player *> m_players;
    };
}

#pragma once
#include <string>
#include <vector>

#include "tile.h"

namespace world {
    class World {
    public:
        explicit World(std::string name);
        ~World() = default;

        void generate();

        std::size_t calculate_memory_usage();
        std::vector<uint8_t> pack_to_memory();

    private:
        uint16_t m_version;
        std::string m_name;
        std::vector<Tile*> m_tiles;
    };
}

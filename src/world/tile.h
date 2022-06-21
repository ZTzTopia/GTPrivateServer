#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace world {
    class Tile {
    public:
        Tile() = default;
        ~Tile() = default;

        std::size_t calculate_memory_usage();
        std::vector<uint8_t> pack_to_memory(std::size_t& size);

    public:
        glm::tvec2<uint8_t> m_position;
        uint16_t m_foreground;
        uint16_t m_background;
    };
}

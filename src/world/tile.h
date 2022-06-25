#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace world {
    class Tile {
    public:
        Tile();
        ~Tile() = default;

        std::size_t calculate_memory_usage();
        std::vector<uint8_t> pack_to_memory();

    public:
        glm::tvec2<uint8_t> m_position;
        uint16_t m_foreground;
        uint16_t m_background;
        union {
            uint16_t val;
            struct {
                uint16_t is_extra : 1;
                uint16_t is_locked: 1;
                uint16_t unk : 1;
                uint16_t unk2 : 1;
                uint16_t is_seed : 1;
                uint16_t is_flipped : 1;
                uint16_t is_open : 1;
                uint16_t is_public : 1;
                uint16_t is_water : 1;
                uint16_t is_glue : 1;
                uint16_t is_fire : 1;
                uint16_t is_red : 1;
                uint16_t is_green : 1;
                uint16_t is_blue : 1;
            };
        } m_flags;
    };
}

#include "world.h"
#include "../utils/random.h"

namespace world {
    void World::generate_new_world(uint32_t width, uint32_t height) {
        auto rand_generator = random::get_generator_static();
        int main_door_pos_x = rand_generator.uniform(static_cast<uint32_t>(1), width - 1);

        m_tiles.reserve(width * height);
        for (int i = 0; i < width * height; i++) {
            Tile *tile = new Tile{};
            tile->m_fg = 0;
            tile->m_bg = 0;

            if (i == 3600 + main_door_pos_x) {
                tile->m_fg = 8; // 6
            }
            else if (i == 3700 + main_door_pos_x) {
                tile->m_fg = 8;
            }
            else if (i >= 3800 && i < 5400 && rand_generator.uniform(0, 100) < 4) {
                tile->m_fg = 10;
            }
            else if (i >= 3700 && i < 5400) {
                if (i > 5000 && rand_generator.uniform(0, 100) < 40) {
                    tile->m_fg = 4;
                }
                else {
                    tile->m_fg = 2;
                }
            }
            else if (i >= 5400) {
                tile->m_fg = 8;
            }

            if (i >= 3700) {
                tile->m_bg = 14;
            }

            m_tiles.push_back(tile);
        }
    }

    uint8_t *World::serialize_to_mem(uint32_t *size_out, uint8_t *dest) {
        uint32_t mem_need = 20 + 4 + 16;
        for (auto &tile : m_tiles) {
            mem_need += 8;
        }

        if (dest == nullptr) {
            dest = new uint8_t[mem_need];
        }

        uint16_t version = 0xF;
        std::memcpy(dest, &version, 2);
        uint32_t mem_pos = 2;

        uint32_t reserved = 0;
        std::memcpy(dest + mem_pos, &reserved, 4);
        mem_pos += 4;

        std::string memeememafmas{ "dek1" };
        uint16_t len = memeememafmas.length();
        std::memcpy(dest + mem_pos, &len, 2);
        mem_pos += 2;

        std::memcpy(dest + mem_pos, memeememafmas.c_str(), len);
        mem_pos += len;

        uint32_t width = 100;
        std::memcpy(dest + mem_pos, &width, 4);
        mem_pos += 4;

        uint32_t height = 60;
        std::memcpy(dest + mem_pos, &height, 4);
        mem_pos += 4;

        int tile_size = m_tiles.size();
        std::memcpy(dest + mem_pos, &tile_size, 4);
        mem_pos += 4;
        
        for (auto &tile : m_tiles) {
            std::memcpy(dest + mem_pos, &tile->m_fg, 2);
            mem_pos += 2;

            std::memcpy(dest + mem_pos, &tile->m_bg, 2);
            mem_pos += 2;

            uint32_t m_lp = 0;
            std::memcpy(dest + mem_pos, &m_lp, 2);
            mem_pos += 2;

            uint32_t m_fl = 0;
            std::memcpy(dest + mem_pos, &m_fl, 2);
            mem_pos += 2;
        }

        uint32_t oc = 0;
        std::memcpy(dest + mem_pos, &oc, 4);
        mem_pos += 4;

        uint32_t drop_id = 0;
        std::memcpy(dest + mem_pos, &drop_id, 4);
        mem_pos += 4;

        uint32_t weather = 4;
        std::memcpy(dest + mem_pos, &weather, 4);
        mem_pos += 4;

        uint32_t weather1 = 0;
        std::memcpy(dest + mem_pos, &weather1, 4);
        mem_pos += 4;

        *size_out = mem_pos;
        return dest;
    }
}
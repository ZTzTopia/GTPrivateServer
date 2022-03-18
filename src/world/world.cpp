#include "world.h"
#include "../utils/random.h"

namespace world {
    // 24 * 100 = 2400
    // 54 * 100 = 5400
    // Dirt block break hit = 3

    World::World(const std::string &name)
        : m_name(name)
        , m_width(100)
        , m_height(54)
        , m_total_net_id(0)
        , m_total_object_id(0)
        , m_owner_user_id(0) {}

    World::~World() {
        for (auto &tiles : m_tiles) {
            delete tiles;
        }

        m_tiles.clear();
        m_players.clear();
    }

    void World::send_to_all(std::function<void(player::Player *)> callback) {
        for (auto &player : m_players) {
            callback(player);
        }
    }

    void World::add_player(player::Player *player) {
        m_players.push_back(player);
    }

    void World::remove_player(player::Player *player) {
        m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
        m_players.shrink_to_fit();
    }

    void World::generate(uint16_t width, uint16_t height) {
        auto rand_generator = random::get_generator_static();
        int main_door_pos_x = rand_generator.uniform(width - (width - 1), width - 1);

        m_tiles.reserve(width * height);
        for (int i = 0; i < width * height; i++) {
            Tile *tile = new Tile{};

            if (i == 2400 + main_door_pos_x) {
                tile->set_fg(6);
                tile->set_label("EXIT");

                m_white_door_pos.x = main_door_pos_x;
                m_white_door_pos.y = static_cast<int>((i - main_door_pos_x) / width);
            }
            else if (i == 2500 + main_door_pos_x) {
                tile->set_fg(8);
            }
            else if (i >= 2600 && i < 4800 && rand_generator.uniform(0, 100) < 4) {
                tile->set_fg(10);
            }
            else if (i >= 2500 && i < 4800) {
                if (i > 4400 && rand_generator.uniform(0, 100) < 40) {
                    tile->set_fg(4);
                }
                else {
                    tile->set_fg(2);
                }
            }
            else if (i >= 4800) {
                tile->set_fg(8);
            }

            if (i >= 2500) {
                tile->set_bg(14);
            }

            m_tiles.push_back(tile);
        }
    }

    uint8_t *World::serialize_to_mem(uint32_t *size_out, uint8_t *dest) {
        uint32_t mem_need = 20 + m_name.length() + 16;
        for (auto &tile : m_tiles) {
            mem_need += tile->calculate_memory_needed();
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

        std::string memeememafmas{ m_name };
        uint16_t len = memeememafmas.length();
        std::memcpy(dest + mem_pos, &len, 2);
        mem_pos += 2;

        std::memcpy(dest + mem_pos, memeememafmas.c_str(), len);
        mem_pos += len;

        uint32_t width = 100;
        std::memcpy(dest + mem_pos, &width, 4);
        mem_pos += 4;

        uint32_t height = 54;
        std::memcpy(dest + mem_pos, &height, 4);
        mem_pos += 4;

        int tile_size = m_tiles.size();
        std::memcpy(dest + mem_pos, &tile_size, 4);
        mem_pos += 4;
        
        for (auto &tile : m_tiles) {
            uint16_t fg = tile->get_fg();
            std::memcpy(dest + mem_pos, &fg, 2);
            mem_pos += 2;

            uint16_t bg = tile->get_bg();
            std::memcpy(dest + mem_pos, &bg, 2);
            mem_pos += 2;

            uint32_t m_lp = 0;
            std::memcpy(dest + mem_pos, &m_lp, 2);
            mem_pos += 2;

            uint32_t flags = tile->get_flags();
            std::memcpy(dest + mem_pos, &flags, 2);
            mem_pos += 2;

            if ((flags & TILEFLAG_TILEEXTRA) == 1) {
                dest[mem_pos++] = 1;

                uint16_t len = static_cast<uint16_t>(tile->get_label().size());
                std::memcpy(dest + mem_pos, &len, 2);
                mem_pos += 2;

                std::memcpy(dest + mem_pos, tile->get_label().c_str(), len);
                mem_pos += len;

                dest[mem_pos++] = 0;
            }
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

    Tile *World::get_tile(int x, int y) {
        if (x < 0 || y < 0 || x > 100 || y > 60) {
            return nullptr;
        }

        return m_tiles[x + y * 100];
    }
}
#include <iostream>
#include <sstream>
#include <utility>

#include "world.h"
#include "../mariadb/conn.h"
#include "../utils/random.h"

namespace world {
    // 24 * 100 = 2400
    // 54 * 100 = 5400
    // Dirt block break hit = 3

    World::World(std::string name)
        : m_name(std::move(name))
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

    void World::send_to_all(const std::function<void(player::Player *)> &callback) {
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

        uint32_t mem_need = 0;
        for (auto &tile : m_tiles) {
            if (tile->get_front_id() == 0) {
                continue;
            }

            mem_need += 4; // The tile position (x + (y * width))
            mem_need += tile->calculate_memory_needed();
        }

        auto *data = new uint8_t[mem_need];
        uint32_t mem_pos = 0;

        for (uint32_t i = 0; i < m_tiles.size(); i++) {
            Tile *tile = m_tiles[i];
            if (tile->get_front_id() == 0) {
                continue;
            }

            std::memcpy(data + mem_pos, &i, 4);
            mem_pos += 4;

            uint16_t fg = tile->get_fg();
            std::memcpy(data + mem_pos, &fg, 2);
            mem_pos += 2;

            uint16_t bg = tile->get_bg();
            std::memcpy(data + mem_pos, &bg, 2);
            mem_pos += 2;

            uint32_t m_lp = 0;
            std::memcpy(data + mem_pos, &m_lp, 2);
            mem_pos += 2;

            uint32_t flags = tile->get_flags();
            std::memcpy(data + mem_pos, &flags, 2);
            mem_pos += 2;

            if ((flags & TILEFLAG_TILEEXTRA) == 1) {
                data[mem_pos++] = 1;

                uint16_t len = static_cast<uint16_t>(tile->get_label().size());
                std::memcpy(data + mem_pos, &len, 2);
                mem_pos += 2;

                std::memcpy(data + mem_pos, tile->get_label().c_str(), len);
                mem_pos += len;

                data[mem_pos++] = 0;
            }
        }

        try {
            sql::PreparedStatement *stmnt(mariadb::get_sql_connection()->prepareStatement("INSERT INTO worlds (name, width, height, data) VALUES (?, ?, ?, ?)"));
            stmnt->setString(1, m_name.c_str());
            stmnt->setUInt(2, width);
            stmnt->setUInt(3, height);

            std::stringstream in(std::string{ reinterpret_cast<char *>(data), mem_pos });
            stmnt->setBlob(4, &in);

            stmnt->executeQuery();
        }
        catch(sql::SQLException& e) {
            spdlog::error("Error inserting new world: {}", e.what());
        }
    }

    bool World::load(const std::string &name) {
        try {
            sql::Statement *stmnt(mariadb::get_sql_connection()->createStatement());
            std::string fmt = fmt::format("SELECT * FROM worlds WHERE name = '{}'", name);
            sql::ResultSet *res = stmnt->executeQuery(fmt.c_str());

            while (res->next()) {
                m_name = std::string{ res->getString(1).c_str() };
                m_width = res->getUInt(2);
                m_height = res->getUInt(3);
                auto out = res->getBlob(4);

                std::streamoff begin = out->tellg();
                out->seekg(0, std::istream::end);
                std::streamoff end = out->tellg();
                out->seekg(0, std::istream::beg);
                auto data_size = static_cast<uint32_t>(end - begin);

                auto *data = new char[data_size];
                out->read(reinterpret_cast<char *>(data), data_size);
                uint32_t mem_pos = 0;

                m_tiles.reserve(m_width * m_height);
                for (uint32_t i = 0; i < m_width * m_height; i++) {
                    uint64_t tile_pos = 0;
                    std::memcpy(&tile_pos, data + mem_pos, 4);
                    if (tile_pos != i) {
                        Tile *tile = new Tile{};
                        m_tiles.push_back(tile);
                        continue;
                    }

                    Tile *tile = new Tile{};

                    mem_pos += 4;

                    uint16_t fg = 0;
                    std::memcpy(&fg, data + mem_pos, 2);
                    tile->set_fg(fg);
                    mem_pos += 2;

                    uint16_t bg = 0;
                    std::memcpy(&bg, data + mem_pos, 2);
                    tile->set_bg(bg);
                    mem_pos += 2;

                    uint16_t lp = 0;
                    std::memcpy(&lp, data + mem_pos, 2);
                    mem_pos += 2;

                    uint16_t flags = 0;
                    std::memcpy(&flags, data + mem_pos, 2);
                    tile->set_flags(flags);
                    mem_pos += 2;

                    if ((tile->get_flags() & TILEFLAG_TILEEXTRA) == 1) {
                        mem_pos++;

                        uint16_t len = 0;
                        std::memcpy(&len, data + mem_pos, 2);
                        mem_pos += 2;

                        std::string label(reinterpret_cast<char *>(data + mem_pos), len);
                        tile->set_label(label);
                        mem_pos += len;

                        mem_pos++;
                    }

                    m_tiles.push_back(tile);
                }

                return true;
            }
        }
        catch(sql::SQLException& e) {
            spdlog::error("Error selecting world: {}", e.what());
        }

        return false;
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

    CL_Vec2i World::get_tile_pos(uint16_t id) {
        for (int i = 0; i < m_tiles.size(); i++) {
            if (m_tiles[i]->get_front_id() == id) {
                return { i % m_width, i / m_width };
            }
        }

        return { -1, -1 };
    }

    CL_Vec2i World::get_tile_fg_pos(uint16_t id) {
        for (int i = 0; i < m_tiles.size(); i++) {
            if (m_tiles[i]->get_fg() == id) {
                return { i % m_width, i / m_width };
            }
        }

        return { -1, -1 };
    }

    CL_Vec2i World::get_tile_bg_pos(uint16_t id) {
        for (int i = 0; i < m_tiles.size(); i++) {
            if (m_tiles[i]->get_fg() == id) {
                return { i % m_width, i / m_width };
            }
        }

        return { -1, -1 };
    }
}
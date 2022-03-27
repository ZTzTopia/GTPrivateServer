#include <utility>

#include "world.h"
#include "../database/database.h"
#include "../../vendor/proton/shared/util/ResourceUtils.h"
#include "../utils/math.h"
#include "../utils/random.h"

namespace world {
    // 24 * 100 = 2400
    // 54 * 100 = 5400
    // Dirt block break hit = 3

    World::World(std::string name)
        : m_last_tile_hash(0)
        , m_name(std::move(name))
        , m_width(100)
        , m_height(54)
        , m_total_net_id(0)
        , m_total_object_id(0)
        , m_owner_user_id(0) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::toupper);
    }

    World::~World() {
        for (auto &tiles : m_tiles) {
            delete tiles;
        }

        m_tiles.clear();
        m_players.clear();
    }

    void World::foreach(const std::function<void(player::Player *)> &callback) {
        std::for_each(m_players.cbegin(), m_players.cend(), callback);
    }

    void World::add_player(player::Player *player) {
        m_players.push_back(player);
    }

    void World::remove_player(player::Player *player) {
        m_players.erase(std::remove(m_players.begin(), m_players.end(), player), m_players.end());
        m_players.shrink_to_fit();
    }

    void World::generate(uint16_t width, uint16_t height) {
        auto rand_generator = utils::random::get_generator_static();
        int main_door_pos_x = rand_generator.uniform(width - (width - 1), width - 1);

        m_tiles.reserve(width * height);
        for (int i = 0; i < width * height; i++) {
            Tile *tile = new Tile{};

            if (i == 2400 + main_door_pos_x) {
                tile->set_fg(6);
                tile->set_label("EXIT");
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

        save(true);
        m_last_tile_hash = std::hash<std::vector<Tile *>>{}(m_tiles);
    }

    void World::save(bool insert) {
        if (!insert && std::hash<std::vector<Tile *>>{}(m_tiles) == m_last_tile_hash) {
            return;
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

        int compressed_size;
        data = brotliCompressToMemory(data, static_cast<int>(mem_pos), &compressed_size);

        try {
            sqlpp::mysql::connection *db = database::get_database()->get_connection();

            auto now = sqlpp::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
            std::vector<uint8_t> data_vector(data, data + compressed_size);
            
            const auto worlds = database::Worlds{};
            if (insert) {
                (*db)(insert_into(worlds).set(
                    worlds.name = m_name,
                    worlds.created_at = now,
                    worlds.updated_at = now,
                    worlds.width = m_width,
                    worlds.height = m_height,
                    worlds.decompressed_size_data = mem_pos,
                    worlds.compressed_size_data = compressed_size,
                    worlds.data = data_vector
                ));
            }
            else {
                (*db)(update(worlds).set(
                    worlds.width = m_width,
                    worlds.updated_at = now,
                    worlds.height = m_height,
                    worlds.decompressed_size_data = mem_pos,
                    worlds.compressed_size_data = compressed_size,
                    worlds.data = data_vector
                ).where(worlds.name == m_name));
            }
        }
        catch(const std::exception &e) {
            spdlog::error("Error inserting new world: {}", e.what());
        }

        delete[] data;
    }

    bool World::load() {
        try {
            sqlpp::mysql::connection *db = database::get_database()->get_connection();

            database::Worlds worlds{};
            for (const auto &row : (*db)(select(all_of(worlds)).from(worlds).where(worlds.name == m_name))) {
                if (row._is_valid) {
                    uint8_t *data = brotliDecompressToMemory((uint8_t *)row.data.blob, static_cast<int>(row.compressed_size_data), static_cast<int>(row.decompressed_size_data));

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

                    m_last_tile_hash = std::hash<std::vector<Tile *>>{}(m_tiles);

                    delete[] data;
                    return true;
                }
            }
        }
        catch(const std::exception &e) {
            spdlog::error("Error selecting world: {}", e.what());
        }

        return false;
    }

    void World::add_object(Object *object) {
        for (auto &object_ : m_objects) {
            printf("%X %X\n", object_, object);
            if (utils::math::distance_xy<int>(object->pos, object_.second->pos) <= 32) {
                return;
            }
        }

        auto rand_generator = utils::random::get_generator_static();
        int x = object->pos.x + rand_generator.uniform(0, 12);
        int y = object->pos.y + rand_generator.uniform(0, 12);

        if (x < 0) {
            x = 0;
        }

        if (y < 0) {
            y = 0;
        }

        if (x >= m_width * 32) {
            x = object->pos.x;
        }

        if (y >= m_height * 32) {
            y = object->pos.y;
        }

        object->pos = { x, y };

        m_objects.insert_or_assign(m_total_object_id++, std::move(object));

        player::GameUpdatePacket game_update_packet{};
        game_update_packet.packet_type = player::PACKET_ITEM_CHANGE_OBJECT;
        game_update_packet.object_change_type = Object::OBJECT_CHANGE_TYPE_ADD;
        game_update_packet.pos_x = static_cast<float>(object->pos.x);
        game_update_packet.pos_y = static_cast<float>(object->pos.y);
        game_update_packet.item_id = object->object_id;
        game_update_packet.object_amount = object->object_amount;

        foreach([&game_update_packet](player::Player *player_) {
            player_->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet);
        });
    }

    Tile *World::get_tile(uint16_t x, uint16_t y) {
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
}
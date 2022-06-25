#include "world.h"
#include "../utils/binary_writer.h"
#include "../utils/random.h"

namespace world {
    World::World(std::string name) : m_version{ 20 }, m_name{ std::move(name) }, m_net_id{ 0 } {}

    void World::generate()
    {
        static randutils::pcg_rng gen{ utils::random::get_generator_local() };

        m_tiles.reserve(100 * 60);
        for (uint32_t i = 0; i < 100 * 60; ++i) {
            glm::tvec2<uint8_t> pos{ i % 100, std::floor(i / 100) };

            Tile* tile = new Tile{};
            tile->m_position = pos;

            if (pos.y > 24 && pos.y < 52) {
                if (pos.y > 47 && gen.uniform(0.0f, 1.0f) < 0.4f) {
                    tile->m_foreground = 4; // Lava
                }
                else {
                    if (gen.uniform(0.0f, 1.0f) < 0.04f) {
                        tile->m_foreground = 10; // Rock
                    }
                    else {
                        tile->m_foreground = 2; // Dirt
                    }
                }
            }
            else if (pos.y > 24) {
                tile->m_foreground = 8; // Bedrock
            }

            if (pos.y > 24) {
                tile->m_background = 14; // Cave
            }

            m_tiles.push_back(tile);
        }
    }

    std::size_t World::calculate_memory_usage()
    {
        std::size_t size{ sizeof(uint16_t) }; // version
        size += sizeof(uint32_t); // reserved
        size += sizeof(uint16_t); // name length
        size += m_name.length(); // name
        size += sizeof(uint32_t); // width
        size += sizeof(uint32_t); // height
        size += sizeof(uint32_t); // tile count

        for (Tile* tile : m_tiles) {
            size += tile->calculate_memory_usage();
        }

        size += sizeof(uint32_t); // object count
        size += sizeof(uint32_t); // object last id
        size += sizeof(uint32_t); // unknown
        size += sizeof(uint32_t); // unknown
        return size;
    }

    std::vector<uint8_t> World::pack_to_memory()
    {
        std::vector<uint8_t> mem(calculate_memory_usage(), 0);

        BinaryWriter writer{ mem.data() };
        writer.write_u16(m_version);
        writer.write_u32(0); // reserved
        writer.write_string(m_name);
        writer.write_u32(100); // width
        writer.write_u32(60); // height
        writer.write_u32(m_tiles.size()); // tile count

        for (Tile* tile : m_tiles) {
            std::vector<uint8_t> data{ tile->pack_to_memory() };
            writer.write_bytes(data.data(), data.size());
        }

        writer.write_u32(0); // object count
        writer.write_u32(0); // object last id
        writer.write_u32(1); // unknown
        writer.write_u32(1); // unknown
        return mem;
    }

    uint32_t World::add_player(const std::shared_ptr<player::Player>& player)
    {
        m_players[m_net_id] = player;
        return ++m_net_id;
    }

    void World::remove_player(const std::shared_ptr<player::Player>& player)
    {
        m_players.erase(player->get_net_id());
    }
}

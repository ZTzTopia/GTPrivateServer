#include "tile.h"
#include "../utils/binary_writer.h"

namespace world {
    Tile::Tile() : m_position{}, m_foreground{ 0 }, m_background{ 0 }, m_flags{} {}

    std::size_t Tile::calculate_memory_usage()
    {
        std::size_t size{ sizeof(uint16_t) }; // foreground
        size += sizeof(uint16_t); // background
        size += sizeof(uint16_t); // parent
        size += sizeof(uint16_t); // flags
        return size;
    }

    std::vector<uint8_t> Tile::pack_to_memory()
    {
        std::vector<uint8_t> mem(calculate_memory_usage(), 0);

        BinaryWriter writer{ mem.data() };
        writer.write_u16(m_foreground);
        writer.write_u16(m_background);
        writer.write_u16(0); // parent
        writer.write_u16(m_flags.val); // flags
        return mem;
    }
}

#include "tile.h"
#include "../utils/binary_writer.h"

namespace world {
    std::size_t Tile::calculate_memory_usage()
    {
        std::size_t size = sizeof(m_foreground);
        size += sizeof(m_background);
        size += sizeof(uint16_t); // parent
        size += sizeof(uint16_t); // flags
        return size;
    }

    std::vector<uint8_t> Tile::pack_to_memory(std::size_t& size)
    {
        std::vector<uint8_t> mem(calculate_memory_usage(), 0);

        BinaryWriter writer{ mem.data() };
        writer.write_u16(m_foreground);
        writer.write_u16(m_background);
        writer.write_u16(0); // parent
        writer.write_u16(0); // flags

        size = writer.position();
        return mem;
    }
}

#include "tile_extra.h"
#include "../utils/binary_writer.h"

namespace world {
    std::size_t TileExtra::calculate_memory_usage()
    {
        std::size_t size = sizeof(uint16_t); // label size
        size += m_label.size();
        size += sizeof(uint32_t); // reserved
        return size;
    }

    std::vector<uint8_t> TileExtra::pack_to_memory()
    {
        std::vector<uint8_t> mem(calculate_memory_usage(), 0);

        BinaryWriter writer{ mem.data() };
        writer.write_string(m_label);
        writer.write_u32(m_unk);
        return mem;
    }
}

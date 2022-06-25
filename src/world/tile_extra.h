#pragma once
#include <array>
#include <string>
#include <vector>

namespace world {
    enum class eTileExtraType : uint8_t {
        NONE,
        DOOR,
        SIGN,
        LOCK,
        SEED,
        MAX
    };

    class TileExtra {
    public:
        TileExtra() = default;
        ~TileExtra() = default;

        std::size_t calculate_memory_usage();
        std::vector<uint8_t> pack_to_memory();

    public:
        std::string m_label;
        uint32_t m_unk;
    };
}

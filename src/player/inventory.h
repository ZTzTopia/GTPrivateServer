#pragma once
#include <map>

namespace player {
    struct Inventory {
        uint32_t size;
        std::map<uint16_t, uint8_t> items;
    };
}
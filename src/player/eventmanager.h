#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>

#include "player.h"

namespace player {
    class EventManager {
    public:
        EventManager() = default;
        ~EventManager() = default;

        static void load(Player *player);
    };
}

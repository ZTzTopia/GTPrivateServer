#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class input {
    public:
        explicit input(player::Player *player);
        ~input() = default;
    };
}
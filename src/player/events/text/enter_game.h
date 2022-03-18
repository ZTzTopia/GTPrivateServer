#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class enter_game {
    public:
        explicit enter_game(player::Player *player);
        ~enter_game() = default;
    };
}
#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class quit_to_exit {
    public:
        explicit quit_to_exit(player::Player *player);
        ~quit_to_exit() = default;
    };
}
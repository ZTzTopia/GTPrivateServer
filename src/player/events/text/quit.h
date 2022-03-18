#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class quit {
    public:
        explicit quit(player::Player *player);
        ~quit() = default;
    };
}
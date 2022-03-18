#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"
#include "../../packet.h"

namespace events {
    class state {
    public:
        explicit state(player::Player *player);
        ~state() = default;
    };
}
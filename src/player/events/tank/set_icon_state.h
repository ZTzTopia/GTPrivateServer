#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"
#include "../../packet.h"

namespace events {
    class set_icon_state {
    public:
        explicit set_icon_state(player::Player *player);
        ~set_icon_state() = default;
    };
}
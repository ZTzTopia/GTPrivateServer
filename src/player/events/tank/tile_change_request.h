#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"
#include "../../packet.h"

namespace events {
    class tile_change_request {
    public:
        explicit tile_change_request(player::Player *player);
        ~tile_change_request() = default;
    };
}
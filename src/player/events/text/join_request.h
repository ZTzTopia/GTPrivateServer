#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class join_request {
    public:
        explicit join_request(player::Player *player);
        ~join_request() = default;
    };
}
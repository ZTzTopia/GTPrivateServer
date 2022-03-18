#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class requestedName {
    public:
        explicit requestedName(player::Player *player);
        ~requestedName() = default;
    };
}
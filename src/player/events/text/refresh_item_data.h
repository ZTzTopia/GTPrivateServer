#pragma once
#include <string>
#include <vector>

#include "../../eventcontext.h"
#include "../../player.h"

namespace events {
    class refresh_item_data {
    public:
        explicit refresh_item_data(player::Player *player);
        ~refresh_item_data() = default;
    };
}
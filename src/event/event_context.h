#pragma once
#include "../item/itemdb.h"
#include "../player/player.h"

namespace event {
    struct EventContext {
        std::shared_ptr<player::Player> player;
        std::shared_ptr<item::ItemDB> item_db;
    };
}

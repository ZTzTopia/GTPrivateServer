#pragma once
#include "../database/postgres.h"
#include "../item/itemdb.h"
#include "../player/player.h"
#include "../world/world_pool.h"

namespace event {
    struct EventContext {
        std::string text;
        player::GameUpdatePacket* tank;
        std::shared_ptr<player::Player> player;
        std::shared_ptr<item::ItemDB> item_db;
        std::shared_ptr<world::WorldPool> world_pool;
        std::shared_ptr<database::Postgres> postgres;
    };
}

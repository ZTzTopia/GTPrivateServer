#include "quit_to_exit.h"

namespace quit_to_exit {
    void function(const event::EventContext& ctx)
    {
        std::shared_ptr<world::World> world{ ctx.world_pool->get_world(ctx.player->get_world()) };
        if (!world) {
            return;
        }

        world->remove_player(ctx.player);
    }
}

#include "event_pool.h"
#include "text/requested_name.h"
#include "text/refresh_item_data.h"
#include "text/enter_game.h"

namespace event {
    void EventPool::load_events()
    {
        on("quit", [](const EventContext& ctx) { ctx.player->disconnect_later(); });
        on("requestedName", [](const EventContext& ctx) { requested_name::function(ctx); });
        on("refresh_item_data", [](const EventContext& ctx) { refresh_item_data::function(ctx); });
        on("enter_game", [](const EventContext& ctx) { enter_game::function(ctx); });
    }
}

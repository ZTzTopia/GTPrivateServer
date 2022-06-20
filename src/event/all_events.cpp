#include "event_pool.h"
#include "text/requested_name.h"
#include "text/refresh_item_data.h"
#include "text/enter_game.h"

namespace event {
    void EventPool::load_events()
    {
        on("quit", [](const EventContext& context) {});
        on("requestedName", [](const EventContext& context) { requested_name::function(context); });
        on("refresh_item_data", [](const EventContext& context) { refresh_item_data::function(context); });
        on("enter_game", [](const EventContext& context) { enter_game::function(context); });
    }
}

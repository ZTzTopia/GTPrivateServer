#include "event_pool.h"
#include "tank/state.h"
#include "text/enter_game.h"
#include "text/join_request.h"
#include "text/quit_to_exit.h"
#include "text/refresh_item_data.h"
#include "text/requested_name.h"

namespace event {
    void EventPool::load_events()
    {
        // Generic text and Game message events.
        on("quit", [](const EventContext& ctx) { ctx.player->disconnect_later(); });
        on("requestedName", [](const EventContext& ctx) { requested_name::function(ctx); });
        on("refresh_item_data", [](const EventContext& ctx) { refresh_item_data::function(ctx); });
        on("enter_game", [](const EventContext& ctx) { enter_game::function(ctx); });
        on("join_request", [](const EventContext& ctx) { join_request::function(ctx); });
        on("quit_to_exit", [](const EventContext& ctx) { quit_to_exit::function(ctx); });

        // Tank events.
        on(std::to_string(player::PACKET_STATE), [](const EventContext& ctx) { state::function(ctx); });
    }
}

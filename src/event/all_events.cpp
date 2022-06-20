#include <spdlog/spdlog.h>

#include "event_pool.h"
#include "text/requested_name.h"

namespace event {
    void EventPool::load_events()
    {
        on("quit", [](const EventContext& context) {});
        on("requestedName", [](const EventContext& context) { requested_name::function(context); });
    }
}

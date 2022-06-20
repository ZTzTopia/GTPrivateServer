#include <spdlog/spdlog.h>

#include "requested_name.h"

namespace requested_name {
    void function(const event::EventContext& context)
    {
        spdlog::info("requestedName");
    }
}

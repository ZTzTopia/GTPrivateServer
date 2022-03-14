#pragma once
#include <spdlog/spdlog.h>

#include "eventcontext.h"
#include "player.h"

namespace player {
    class GenericTextEvent : public EventContext {
    public:
        explicit GenericTextEvent(Player *player) : EventContext(player) {};
        ~GenericTextEvent() = default;

        void trigger(const std::string &event_name) {
            spdlog::trace("GenericTextEvent::trigger({})", event_name);
        };
    };
}

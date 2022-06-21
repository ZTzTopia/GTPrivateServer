#include "event_pool.h"
#include "../utils/hash.h"
#include "../utils/text_parse.h"

namespace event {
    EventPool::EventPool()
    {
        load_events();
    }

    void EventPool::try_find_and_fire_event(const std::string& event_name, const EventContext& context)
    {
        utils::TextParse text_parse{ event_name };
        if (text_parse.get("action", 1).empty()) {
            // No action specified, so we fire the event with
            // first key as action.
            spdlog::debug("Trying to call event {}.", event_name.substr(0, event_name.find('|')));
            if (!emit(event_name.substr(0, event_name.find('|')), context)) {
                spdlog::warn("Failed to call event {}.", event_name.substr(0, event_name.find('|')));
            }

            return;
        }

        spdlog::debug("Trying to call event {}.", text_parse.get("action", 1));
        if (!emit(text_parse.get("action", 1), context)) {
            spdlog::warn("Failed to call event {}.", text_parse.get("action", 1));
        }
    }
}

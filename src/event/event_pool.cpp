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
            emit(event_name.substr(0, event_name.find('|')), context);
            return;
        }

        emit(text_parse.get("action", 1), context);
    }
}

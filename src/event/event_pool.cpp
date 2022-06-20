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
        printf("Trying to find event: %s\n", event_name.c_str());

        utils::TextParse text_parse{ event_name };
        if (text_parse.get("action", 1).empty()) {
            emit(event_name.substr(0, event_name.find('|')), context);
            return;
        }

        emit(text_parse.get("action", 1), context);
    }
}

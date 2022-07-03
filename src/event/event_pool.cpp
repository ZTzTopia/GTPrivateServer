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
            spdlog::trace("Trying to call event {}.", event_name.substr(0, event_name.find('|')));
            if (!emit(event_name.substr(0, event_name.find('|')), context)) {
                spdlog::trace("Failed to call event {}.", event_name.substr(0, event_name.find('|')));
            }

            return;
        }

        spdlog::trace("Trying to call action event {}.", text_parse.get("action", 1));
        if (!emit(text_parse.get("action", 1), context)) {
            spdlog::trace("Failed to call action event {}.", text_parse.get("action", 1));
        }
    }

    void EventPool::try_find_and_fire_event(player::ePacketType packet_type, const EventContext& context)
    {
        spdlog::trace("Trying to call packet event {}.", magic_enum::enum_name(packet_type));
        if (!emit(std::to_string(packet_type), context)) {
            spdlog::trace("Failed to call packet event {}.", magic_enum::enum_name(packet_type));
        }
    }
}

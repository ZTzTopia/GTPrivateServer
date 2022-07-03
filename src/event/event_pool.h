#pragma once
#include <string>

#include "event_context.h"
#include "../utils/event_emitter.h"

namespace event {
    class EventPool : public utils::EventEmitter {
    public:
        EventPool();
        ~EventPool() = default;

        void try_find_and_fire_event(const std::string& event_name, const EventContext& context);
        void try_find_and_fire_event(player::ePacketType packet_type, const EventContext& context);

    private:
        void load_events();
    };
}
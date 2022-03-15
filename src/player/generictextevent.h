#pragma once
#include <spdlog/spdlog.h>

#include "eventcontext.h"
#include "player.h"

namespace player {
    class GenericTextEvent : public EventContext {
    public:
        explicit GenericTextEvent(Player *player) : EventContext(player) {};
        ~GenericTextEvent() = default;

        void quit();
        void guest();
        void refresh_item_data();
        void enter_game();
        void join_request();
    };
}

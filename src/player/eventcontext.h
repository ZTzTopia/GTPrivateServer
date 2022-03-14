#pragma once
#include "player.h"

namespace player {
    class EventContext {
    public:
        explicit EventContext(Player *player) { m_player = player; }
        ~EventContext() = default;

        Player *get_player() const { return m_player; }

    private:
        Player *m_player;
    };
}
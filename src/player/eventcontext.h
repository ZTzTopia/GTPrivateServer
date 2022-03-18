#pragma once
#include "player.h"

namespace player {
    class EventContextText {
    public:
        explicit EventContextText(Player *player) { m_player = player; }
        ~EventContextText() = default;

    public:
        Player *m_player;
    };
}
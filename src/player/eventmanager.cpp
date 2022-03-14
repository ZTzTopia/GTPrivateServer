#include "eventmanager.h"
#include "generictextevent.h"

namespace player {
    void EventManager::load(Player *player) {
        auto *generic_text_event = new GenericTextEvent{ player };
        player->load("requestedName", [player, generic_text_event]() {
            generic_text_event->trigger("requestedName");
            player->unload("requestedName");
        });
    }
}
#include "eventmanager.h"
#include "generictextevent.h"

namespace player {
    void EventManager::load(Player *player) {
        auto *generic_text_event = new GenericTextEvent{ player };
        player->load("quit", [player, generic_text_event]() {
            generic_text_event->quit();
            // player->unload("quit");
        });
        player->load("requestedName", [player, generic_text_event]() {
            generic_text_event->guest();
            // player->unload("requestedName");
        });
        player->load("refresh_item_data", [generic_text_event]() {
            generic_text_event->refresh_item_data();
        });
        player->load("enter_game", [player, generic_text_event]() {
            generic_text_event->enter_game();
            // player->unload("enter_game");
        });
        player->load("join_request", [player, generic_text_event]() {
            generic_text_event->enter_game();
            // player->unload("enter_game");
        });
    }
}
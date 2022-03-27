#include "enter_game.h"

namespace events {
    enter_game::enter_game(player::Player *player) {
        player->load_once("enter_game", [player](const std::string &string) {
            player->send_inventory();
            player->send_variant({
                "OnRequestWorldSelectMenu",
                "add_filter|\nadd_floater|ANJAYPITERP|0|0.5|2147418367"
            });
            player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", player->get_peer()->host->connectedPeers));
        });
    }
}
#include "quit.h"

namespace events {
    quit::quit(player::Player *player) {
        player->load_once("quit", [player](const std::string &string) {
            enet_peer_disconnect_later(player->get_peer(), 0);
        });
    }
}
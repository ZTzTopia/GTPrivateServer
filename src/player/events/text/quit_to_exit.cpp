#include "quit_to_exit.h"
#include "../../../server/server.h"

namespace events {
    quit_to_exit::quit_to_exit(player::Player *player) {
        player->load("quit_to_exit", [player](const std::string &string) {
            if (player->get_current_world() == "EXIT") {
                return;
            }

            auto *world_pool = player->get_server()->get_world_pool();
            if (!world_pool) {
                return;
            }

            auto *world = world_pool->get_world(player->get_current_world());
            if (!world) {
                return;
            }

            if (world->get_players().size() == 1) {
                world->save();
                world_pool->remove_world(world);
            }
            else {
                world->foreach([player](player::Player *player_) {
                    if (player == player_) {
                        return;
                    }

                    player_->send_variant({
                        "OnRemove",
                        fmt::format(
                            "netID|{}",
                            player->get_net_id()
                        )
                    });
                });
                world->remove_player(player);
            }

            player->set_current_world("EXIT");
            player->send_variant({
                "OnRequestWorldSelectMenu",
                "add_filter|\nadd_floater|ANJAYPITERP|0|0.5|2147418367"
            });
            player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", player->get_peer()->host->connectedPeers));
        });
    }
}
#include "quit_to_exit.h"
#include "../../../server/serverpool.h"

namespace events {
    quit_to_exit::quit_to_exit(player::Player *player) {
        player->load("quit_to_exit", [player](const std::string &string) {
            if (player->m_current_world == "EXIT") {
                return;
            }

            auto *server = server::get_server_pool()->get_server(player->get_server_id());
            if (!server) {
                return;
            }

            auto *world_pool = server->get_world_pool();
            if (!world_pool) {
                return;
            }

            player->send_variant({
                "OnRequestWorldSelectMenu",
                "add_filter|\nadd_floater|ANJAYPITERP|0|0.5|2147418367"
            });

            player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", player->get_peer()->host->connectedPeers));

            auto *world = world_pool->get_world(player->m_current_world);
            if (!world) {
                return;
            }

            player->m_current_world = "EXIT";

            if (world->get_players().size() == 1) {
                world_pool->remove_world(world);
                return;
            }

            world->send_to_all([player](player::Player *other_player) {
                if (player == other_player) {
                    return;
                }

                other_player->send_variant({
                    "OnRemove",
                    fmt::format(
                        "netID|{}",
                        player->m_net_id
                    )
                });
            });

            world->remove_player(player);
        });
    }
}
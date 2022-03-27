#include "input.h"
#include "../../../utils/textparse.h"
#include "../../../server/server.h"

namespace events {
    input::input(player::Player *player) {
        player->load("input", [player](const std::string &string) {
            if (player->get_current_world() == "EXIT") {
                return;
            }

            TextParse text_parse{ string };
            std::string text = text_parse.get("text", 1);

            if (text.empty()) {
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

            // https://stackoverflow.com/a/35301768/13257595
            bool prev_is_space = true;
            text.erase(std::remove_if(text.begin(), text.end(), [&prev_is_space](unsigned char curr) {
                bool r = std::isspace(curr) && prev_is_space;
                prev_is_space = std::isspace(curr);
                return r;
            }), text.end());

            world->foreach([player, text](player::Player *player_) {
                player_->send_variant({
                    "OnConsoleMessage",
                    fmt::format(
                        "CP:0_PL:4_OID:_CT:[W]_ `o<`w{}``> ``{}``",
                        player->get_display_name(),
                        text
                    )
                });

                player_->send_variant({
                    "OnTalkBubble",
                    player->get_net_id(),
                    fmt::format(
                        "`w{}``",
                        text
                    )
                });
            });
        });
    }
}
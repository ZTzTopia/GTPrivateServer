#include "input.h"
#include "../../../utils/textparse.h"
#include "../../../server/serverpool.h"

namespace events {
    input::input(player::Player *player) {
        player->load("input", [player](const std::string &string) {
            if (player->m_current_world == "EXIT") {
                return;
            }

            TextParse text_parse{ string };
            std::string text = text_parse.get("text", 1);

            if (text.empty()) {
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

            auto *world = world_pool->get_world(player->m_current_world);
            if (!world) {
                return;
            }

            world->send_to_all([player, text](player::Player *other_player) {
                other_player->send_variant({
                    "OnConsoleMessage",
                    fmt::format(
                        "CP:0_PL:4_OID:_CT:[W]_ `o<`w{}``> ``{}``",
                        player->m_display_name,
                        text
                    )
                });

                other_player->send_variant({
                    "OnTalkBubble",
                    player->m_net_id,
                    fmt::format(
                        "`w{}``",
                        text
                    )
                });
            });
        });
    }
}
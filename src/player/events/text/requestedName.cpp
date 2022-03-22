#include "requestedName.h"
#include "../../../config.h"
#include "../../../server/serverpool.h"
#include "../../../utils/textparse.h"
#include "../../../utils/random.h"

namespace events {
    requestedName::requestedName(player::Player *player) {
        player->load_once("requestedName", [player](const std::string &string) {
            if (string.empty()) {
                enet_peer_disconnect_later(player->get_peer(), 0);
                return;
            }

            printf("%s\n", string.c_str());
            TextParse text_parse{ string };
            size_t user_hash{ std::hash<std::string>{}(text_parse.get("requestedName", 1)) };
            user_hash += player->get_peer()->host->address.port;

            if (text_parse.get<uint32_t>("lmode", 1) != 3 && text_parse.get<uint32_t>("user", 1) != static_cast<uint32_t>(user_hash)) {
                enet_peer_disconnect_later(player->get_peer(), 0);
                return;
            }

            player->m_raw_name = text_parse.get("requestedName", 1);
            player->m_mac = text_parse.get("mac", 1);
            player->m_country = text_parse.get("country", 1);
            player->m_display_name = "`w" + player->m_raw_name + "_" + std::to_string(random::get_generator_static().uniform(100, 999));

            if (!player->load_()) {
                player->m_user_id = player->save(-1, true);
            }

            if (text_parse.get<uint32_t>("lmode", 1) == 3) {
                auto *world_pool=server::get_server_pool()->get_server(player->get_server_id())->get_world_pool();
                world_pool->get_world(player->m_current_world)->add_player(player);

                player->m_net_id = world_pool->get_world(player->m_current_world)->increase_total_net_id();

                player->send_variant({
                    "OnSpawn",
                    fmt::format(
                        "spawn|avatar\n"
                        "netID|{}\n"
                        "userID|{}\n"
                        "colrect|0|0|20|30\n"
                        "posXY|{}|{}\n"
                        "name|{}\n"
                        "country|{}\n"
                        "invis|0\n"
                        "mstate|0\n"
                        "smstate|1\n"
                        "onlineID|\n"
                        "type|local\n",
                        player->m_net_id,
                        player->m_user_id,
                        world_pool->get_world(player->m_current_world)->get_tile_pos(6).x * 32,
                        world_pool->get_world(player->m_current_world)->get_tile_pos(6).y * 32,
                        player->m_display_name,
                        player->m_country
                    )
                });

                std::string world_name = player->m_current_world;
                world_pool->get_world(player->m_current_world)->send_to_all([player, world_pool, world_name](player::Player *other_player) {
                    if (player == other_player) {
                        return;
                    }

                    other_player->send_variant({
                        "OnSpawn",
                        fmt::format(
                            "spawn|avatar\n"
                            "netID|{}\n"
                            "userID|{}\n"
                            "colrect|0|0|20|30\n"
                            "posXY|{}|{}\n"
                            "name|{}\n"
                            "country|{}\n"
                            "invis|0\n"
                            "mstate|0\n"
                            "smstate|1\n"
                            "onlineID|\n",
                            player->m_net_id,
                            player->m_user_id,
                            world_pool->get_world(world_name)->get_tile_pos(6).x * 32,
                            world_pool->get_world(world_name)->get_tile_pos(6).y * 32,
                            player->m_display_name,
                            player->m_country
                        )
                    });

                    player->send_variant({
                        "OnSpawn",
                        fmt::format(
                            "spawn|avatar\n"
                            "netID|{}\n"
                            "userID|{}\n"
                            "colrect|0|0|20|30\n"
                            "posXY|{}|{}\n"
                            "name|{}\n"
                            "country|{}\n"
                            "invis|0\n"
                            "mstate|0\n"
                            "smstate|1\n"
                            "onlineID|\n",
                            other_player->m_net_id,
                            other_player->m_user_id,
                            other_player->m_pos.x,
                            other_player->m_pos.y,
                            other_player->m_display_name,
                            other_player->m_country
                        )
                    });
                });

                player->send_log(fmt::format("`oWorld `w{} ``entered. There are `w{} ``other people here, `w0 ``online", world_name, world_pool->get_world(world_name)->get_players().size()), true);
            }

            static std::string param2 = std::string{ config::on_super_main::param2 };
            static std::string param3 = std::string{ config::on_super_main::param3 };
            static std::string param4 = std::string{ config::on_super_main::param4 };
            static std::string param5 = std::string{ config::on_super_main::param5 };

            player->send_variant({
                "OnSuperMainStartAcceptLogonHrdxs47254722215a",
                items::get_items_db()->get_hash(),
                param2,
                param3,
                param4,
                param5/*,
                -1*/ // This is player tribute data ok.
            });
        });
    }
}
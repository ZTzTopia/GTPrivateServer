#include "join_request.h"
#include "../../../config.h"
#include "../../../utils/textparse.h"
#include "../../../server/server.h"

namespace events {
    join_request::join_request(player::Player *player) {
        player->load("join_request", [player](const std::string &string) {
            auto *world_pool = player->get_server()->get_world_pool();
            if (!world_pool) {
                return;
            }

            TextParse text_parse{ string };
            std::string world_name = text_parse.get("name", 1);

            if (world_name.empty()) {
                return;
            }

            std::transform(world_name.begin(), world_name.end(), world_name.begin(), ::toupper);

            world::World *world = world_pool->get_world_or_generate(world_name);
            if (!world) {
                return;
            }

            player->set_current_world(world_name);
            player->set_net_id(world->increase_total_net_id());

            world->add_player(player);

            uint32_t data_size;
            uint8_t *data = world->serialize_to_mem(&data_size, nullptr);

            player::GameUpdatePacket game_update_packet{};
            game_update_packet.packet_type = player::PACKET_SEND_MAP_DATA;
            game_update_packet.net_id = -1;
            game_update_packet.flags |= 0x8;
            game_update_packet.data_extended_size = data_size;
            game_update_packet.data_extended = reinterpret_cast<uint32_t&>(data);
            player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

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
                    player->get_net_id(),
                    player->get_user_id(),
                    world->get_tile_pos(6).x * 32,
                    world->get_tile_pos(6).y * 32,
                    player->get_display_name(),
                    player->get_country()
                )
            });

            world->foreach([player, world](player::Player *player_) {
                if (player == player_) {
                    return;
                }

                player_->send_variant({
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
                        player->get_net_id(),
                        player->get_user_id(),
                        world->get_tile_pos(6).x * 32,
                        world->get_tile_pos(6).y * 32,
                        player->get_display_name(),
                        player->get_country()
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
                        player_->get_net_id(),
                        player_->get_user_id(),
                        player_->get_pos().x,
                        player_->get_pos().y,
                        player_->get_display_name(),
                        player_->get_country()
                    )
                });
            });

            player->send_log(fmt::format("`oWorld `w{} ``entered. There are `w{} ``other people here, `w0 ``online", world_name, world->get_players().size()), true);
        });
    }
}
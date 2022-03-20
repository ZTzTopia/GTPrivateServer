#include "join_request.h"
#include "../../../server/serverpool.h"
#include "../../../utils/textparse.h"

namespace events {
    join_request::join_request(player::Player *player) {
        player->load("join_request", [player](const std::string &string) {
            auto *server = server::get_server_pool()->get_server(player->get_server_id());
            if (!server) {
                return;
            }

            auto *world_pool = server->get_world_pool();
            if (!world_pool) {
                return;
            }

            TextParse text_parse{ string };
            std::string world_name = text_parse.get("name", 1);
            std::transform(world_name.begin(), world_name.end(), world_name.begin(), ::toupper);

            uint32_t data_size;
            uint8_t *data = world_pool->get_world_data(world_name, &data_size);

            player->m_current_world = world_name;
            player->m_net_id = world_pool->get_world(world_name)->increase_total_net_id();
            world_pool->get_world(world_name)->add_player(player);

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
                    player->m_net_id,
                    player->m_user_id,
                    world_pool->get_world(world_name)->get_tile_pos(6).x * 32,
                    world_pool->get_world(world_name)->get_tile_pos(6).y * 32,
                    player->m_display_name,
                    player->m_country
                )
            });

            world_pool->get_world(world_name)->send_to_all([player, world_pool, world_name](player::Player *other_player) {
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
                        world_pool->get_world(world_name)->get_tile_pos(6).x * 32,
                        world_pool->get_world(world_name)->get_tile_pos(6).y * 32,
                        other_player->m_display_name,
                        other_player->m_country
                    )
                });
            });
        });
    }
}
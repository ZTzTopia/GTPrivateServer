#include "join_request.h"
#include "../../../config.h"
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

            if (world_name.empty()) {
                return;
            }

            std::transform(world_name.begin(), world_name.end(), world_name.begin(), ::toupper);

            player->m_current_world = world_name;

            world::World *world = nullptr;
            player::Player *new_player = player;
            for (auto &server_ : server::get_server_pool()->get_servers()) {
                if (server_ == server) {
                    continue;
                }

                auto *world_pool_ = server_->get_world_pool();
                if (world_pool_->is_world_loaded(world_name)) {
                    world_pool = world_pool_;
                    world = world_pool_->get_world(world_name);

                    size_t user_hash{ std::hash<std::string>{}(player->m_raw_name) };
                    user_hash += player->get_peer()->host->address.port;
size_t toekn = user_hash + (rand() % 1012310);
                    player->send_variant({
                        "OnSendToServer",
                        server_->get_port(),
                        (int32_t)toekn,
                        static_cast<int32_t>(user_hash),
                        fmt::format("{}|", config::address),
                        3
                    });

                    player->m_on_send_to_server = true;
                    player->save();
                    // enet_peer_disconnect_later(player->get_peer(), 0);
                    break;
                }
            }

            if (!world) {
                world = world_pool->get_world_or_generate(world_name);
                if (!world) {
                    return;
                }
            }

            if (!player->m_on_send_to_server) {
                new_player->m_net_id = world_pool->get_world(world_name)->increase_total_net_id();

                world->add_player(new_player);
            }

            uint32_t data_size;
            uint8_t *data = world_pool->get_world_data(world_name, &data_size);

            player::GameUpdatePacket game_update_packet{};
            game_update_packet.packet_type = player::PACKET_SEND_MAP_DATA;
            game_update_packet.net_id = -1;
            game_update_packet.flags |= 0x8;
            game_update_packet.data_extended_size = data_size;
            game_update_packet.data_extended = reinterpret_cast<uint32_t&>(data);
            new_player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

            if (!player->m_on_send_to_server) {
                new_player->send_variant({
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
                        new_player->m_net_id,
                        new_player->m_user_id,
                        world_pool->get_world(world_name)->get_tile_pos(6).x * 32,
                        world_pool->get_world(world_name)->get_tile_pos(6).y * 32,
                        new_player->m_display_name,
                        new_player->m_country
                    )
                });

                world->send_to_all([new_player, world_pool, world_name](player::Player *other_player) {
                    if (new_player == other_player) {
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
                            new_player->m_net_id,
                            new_player->m_user_id,
                            world_pool->get_world(world_name)->get_tile_pos(6).x * 32,
                            world_pool->get_world(world_name)->get_tile_pos(6).y * 32,
                            new_player->m_display_name,
                            new_player->m_country
                        )
                    });

                    new_player->send_variant({
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

                player->send_log(fmt::format("`oWorld `w{} ``entered. There are `w{} ``other people here, `w0 ``online", world_name, world->get_players().size()), true);
            }
        });
    }
}
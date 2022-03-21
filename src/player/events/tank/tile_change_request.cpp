#include "tile_change_request.h"
#include "../../../server/serverpool.h"

namespace events {
    tile_change_request::tile_change_request(player::Player *player) {
        player->load("gup_" + std::to_string(player::PACKET_TILE_CHANGE_REQUEST), [player](player::GameUpdatePacket *game_update_packet) {
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

            auto *world = world_pool->get_world(player->m_current_world);
            if (!world) {
                return;
            }

            auto *tile = world->get_tile(game_update_packet->m_tile_pos_x, game_update_packet->m_tile_pos_y);
            if (!tile) {
                return;
            }

            switch (game_update_packet->item_id) {
                case 18: {
                    if (tile->get_item_info()->id == 0) {
                        return;
                    }

                    if (tile->get_item_info()->id == 6 || tile->get_item_info()->id == 8) {
                        player->send_packet(player::NET_MESSAGE_GAME_MESSAGE, "action|play_sfx\nfile|audio/cant_break_tile.wav\ndelayMS|0\n");
                        player->send_variant({
                            "OnTalkBubble",
                            player->m_net_id,
                            "It's too strong to break.",
                            0,
                            1
                        });
                        return;
                    }

                    tile->reset_hit_count_by_time();

                    game_update_packet->packet_type = player::PACKET_TILE_APPLY_DAMAGE;
                    game_update_packet->net_id = player->m_net_id;
                    game_update_packet->tile_damage = 6;

                    // Dirt = 3 hit
                    // Lava = 4 hit
                    // Rock = 10 hit
                    if (tile->get_hit_count() > ((tile->get_item_info()->break_hits / 6) <= 4 ? tile->get_item_info()->break_hits / 8 : tile->get_item_info()->break_hits / 6.5)) {
                        game_update_packet->packet_type = player::PACKET_TILE_CHANGE_REQUEST;
                        game_update_packet->item_id = 18;
                        tile->remove_tile();
                    }

                    world->send_to_all([player, game_update_packet](player::Player *other_player) {
                        other_player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, game_update_packet);
                    });
                }
            }
        });
    }
}
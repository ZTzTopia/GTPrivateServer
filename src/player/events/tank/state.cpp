#include "state.h"
#include "../../../server/server.h"

namespace events {
    state::state(player::Player *player) {
        player->load("gup_" + std::to_string(player::PACKET_STATE), [player](player::GameUpdatePacket *game_update_packet) {
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

            player->set_pos(game_update_packet->pos_x, game_update_packet->pos_y);

            game_update_packet->net_id = player->get_net_id();
            world->foreach([game_update_packet](player::Player *player_) {
                player_->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, game_update_packet);
            });
        });
    }
}
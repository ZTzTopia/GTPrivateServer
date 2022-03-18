#include "refresh_item_data.h"
#include "../../../config.h"
#include "../../../items/itemsdb.h"

namespace events {
    refresh_item_data::refresh_item_data(player::Player *player) {
        player->load_once("refresh_item_data", [player](const std::string &string) {
            player->send_log("One moment, updating item data...", true);

            player::GameUpdatePacket game_update_packet{};
            game_update_packet.packet_type = player::PACKET_SEND_ITEM_DATABASE_DATA;
            game_update_packet.net_id = -1;
            game_update_packet.flags |= 0x8;
            game_update_packet.dec_item_data_size = items::get_items_db()->get_size();
            game_update_packet.data_extended_size = items::get_items_db()->get_compressed_size();
            player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, items::get_items_db()->get_compressed_data());
        });
    }
}
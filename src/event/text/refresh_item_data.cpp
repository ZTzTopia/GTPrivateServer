#include "refresh_item_data.h"

namespace refresh_item_data {
    void function(const event::EventContext& ctx)
    {
        ctx.player->send_log("One moment, updating item data...", true);

        player::GameUpdatePacket game_update_packet{};
        game_update_packet.type = player::PACKET_SEND_ITEM_DATABASE_DATA;
        game_update_packet.net_id = -1;
        game_update_packet.flags |= 0x8;
        game_update_packet.dec_item_data_size = ctx.item_db->get_size();
        game_update_packet.data_size = ctx.item_db->get_compressed_size();
        ctx.player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket), ctx.item_db->get_compressed_data().data());
    }
}

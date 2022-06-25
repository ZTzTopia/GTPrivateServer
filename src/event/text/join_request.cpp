#include <regex>

#include "enter_game.h"
#include "../../utils/text_parse.h"

namespace join_request {
    void function(const event::EventContext& ctx)
    {
        utils::TextParse text_parse{ ctx.text };
        std::string world_name = text_parse.get("name", 1);
        if (world_name.empty()) {
            ctx.player->send_variant({ "OnFailedToEnterWorld", 1 });
            return;
        }

        // Transform the world name to uppercase.
        std::transform(world_name.begin(), world_name.end(), world_name.begin(), ::toupper);

        std::regex regex{ "^[A-Z0-9]+$" }; // Only allow alphanumeric characters.
        if (world_name == "EXIT" || !std::regex_match(world_name, regex)) {
            ctx.player->send_variant({ "OnFailedToEnterWorld", 1 });
            return;
        }

        std::shared_ptr<world::World> world{ ctx.world_pool->get_world(world_name) };
        if (!world) {
            world = ctx.world_pool->new_world(world_name);
        }

        std::vector<uint8_t> data{ world->pack_to_memory() };

        player::GameUpdatePacket game_update_packet{};
        game_update_packet.type = player::PACKET_SEND_MAP_DATA;
        game_update_packet.net_id = -1;
        game_update_packet.flags |= player::ePacketFlag::PACKET_FLAG_EXTENDED;
        game_update_packet.data_size = data.size();
        ctx.player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket), data.data());

        uint32_t net_id = world->add_player(ctx.player);
        ctx.player->set_net_id(net_id);

        ctx.player->send_variant({
            "OnSpawn",
            fmt::format(
                "spawn|avatar\n"
                "netID|{}\n"
                "userID|1\n"
                "colrect|0|0|20|30\n"
                "posXY|32|32\n"
                "name|ztz\n"
                "country|us\n"
                "invis|0\n"
                "mstate|0\n"
                "smstate|1\n"
                "onlineID|1\n"
                "type|local\n", net_id)
        });
    }
}

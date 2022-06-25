#include "state.h"

namespace state {
    void function(const event::EventContext& ctx)
    {
        ctx.tank->net_id = ctx.player->get_net_id();
        ctx.player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, ctx.tank);
    }
}

#include "state.h"

namespace state {
    void function(const event::EventContext& ctx)
    {
        ctx.tank->net_id = 1;
        ctx.player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, ctx.tank);
    }
}

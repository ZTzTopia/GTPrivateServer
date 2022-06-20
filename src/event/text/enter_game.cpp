#include "enter_game.h"

namespace enter_game {
    void function(const event::EventContext& ctx)
    {
        ctx.player->send_variant({ "OnRequestWorldSelectMenu", "add_filter|\nadd_floater|START|0|0.5|2147418367" });
        ctx.player->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", ctx.player->get_peer()->host->connectedPeers));
    }
}

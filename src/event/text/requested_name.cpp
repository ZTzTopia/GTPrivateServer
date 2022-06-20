#include "requested_name.h"

namespace requested_name {
    void function(const event::EventContext& ctx)
    {
        ctx.player->send_variant({
             "OnSuperMainStartAcceptLogonHrdxs47254722215a",
             ctx.item_db->get_hash(),
             "ubistatic-a.akamaihd.net",
             "0098/18818/cache",
             "",
             "proto=162|choosemusic=audio/mp3/about_theme.mp3|active_holiday=2|wing_week_day=0|ubi_week_day=0|server_tick=83430103|clash_active=1|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|",
             -1
        });
    }
}

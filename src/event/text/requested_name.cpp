#include "requested_name.h"
#include "../../utils/text_parse.h"

namespace requested_name {
    void function(const event::EventContext& ctx)
    {
        utils::TextParse text_parse{ ctx.text };
        std::vector<uint8_t> rid;
        for (auto& c : text_parse.get("rid", 1)) {
            rid.push_back(c);
        }
        std::vector<uint8_t> wk;
        for (auto& c : text_parse.get("wk", 1)) {
            wk.push_back(c);
        }

        ctx.postgres->m_player.accounts_s_all->params.rid = rid;
        auto rows = ctx.postgres->operator()(*ctx.postgres->m_player.accounts_s_all);
        if (rows.empty()) {
            ctx.postgres->m_player.accounts_i->params.name = text_parse.get("requestedName", 1);
            ctx.postgres->m_player.accounts_i->params.guest = true;
            ctx.postgres->m_player.accounts_i->params.mac = text_parse.get("mac", 1);
            ctx.postgres->m_player.accounts_i->params.rid = rid;
            ctx.postgres->m_player.accounts_i->params.wk = wk;
            ctx.postgres->operator()(*ctx.postgres->m_player.accounts_i);
        }

        ctx.player->send_variant({
             "OnSuperMainStartAcceptLogonHrdxs47254722215a",
             ctx.item_db->get_hash(),
             "ubistatic-a.akamaihd.net",
             "0098/33811/cache/",
             "cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster",
             "proto=163|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|wing_week_day=0|ubi_week_day=0|server_tick=7077939|clash_active=1|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|",
             -1
        });
    }
}

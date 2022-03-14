#pragma once
#include <spdlog/spdlog.h>

#include "eventcontext.h"
#include "player.h"
#include "../items/itemsdb.h"
#include "../proton/shared/util/Variant.h"

namespace player {
    class GenericTextEvent : public EventContext {
    public:
        explicit GenericTextEvent(Player *player) : EventContext(player) {};
        ~GenericTextEvent() = default;

        void trigger(const std::string &event_name) {
            VariantList variant_list;
            variant_list.Get(0).Set("OnSuperMainStartAcceptLogonHrdxs47254722215a");
            variant_list.Get(1).Set(items::get_items_db()->get_hash());
            variant_list.Get(2).Set("ubistatic-a.akamaihd.net");
            variant_list.Get(3).Set("0098/313646/cache/");
            variant_list.Get(4).Set("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
            variant_list.Get(5).Set("proto=158|choosemusic=audio/mp3/about_theme.mp3|active_holiday=2|wing_week_day=0|ubi_week_day=0|server_tick=83654979|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|");

            uint32_t data_size;
            uint8_t *data = variant_list.SerializeToMem(&data_size, nullptr);

            GameUpdatePacket packet{};
            packet.packet_type = PACKET_CALL_FUNCTION;
            packet.netid = -1;
            packet.flags |= 0x8;
            packet.data_extended_size = data_size;
            get_player()->send_raw_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(GameUpdatePacket), data, ENET_PACKET_FLAG_RELIABLE);

            delete[] data;
        };
    };
}

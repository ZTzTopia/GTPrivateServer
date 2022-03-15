#include <spdlog/spdlog.h>

#include "generictextevent.h"
#include "player.h"
#include "../items/itemsdb.h"

namespace player {
    void GenericTextEvent::quit() {
        enet_peer_disconnect_later(get_player()->get_peer(), 0);
    }

    void GenericTextEvent::guest() {
        VariantList variant_list{};
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
        packet.net_id = -1;
        packet.flags |= 0x8;
        packet.data_extended_size = data_size;
        get_player()->send_raw_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

        delete[] data;
    }

    void GenericTextEvent::refresh_item_data() {
        get_player()->send_packet(NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|One moment, updating item data..."));

        GameUpdatePacket packet{};
        packet.packet_type = PACKET_SEND_ITEM_DATABASE_DATA;
        packet.net_id = -1;
        packet.flags |= 0x8;
        packet.unk8 = items::get_items_db()->get_size();
        packet.data_extended_size = items::get_items_db()->get_compressed_size();
        get_player()->send_raw_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(GameUpdatePacket) - 4, items::get_items_db()->get_compressed_data(), ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
    }

    void GenericTextEvent::enter_game() {
        get_player()->send_variant(VariantList{
            "OnRequestWorldSelectMenu",
            "add_filter|\nadd_floater|ANJAYPITERP|0|0.5|2147418367"
        });
        get_player()->send_packet(NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|Where would you like to go? (`w{}`` others online)", get_player()->get_peer()->host->connectedPeers));
    }

    void GenericTextEvent::join_request() {
        VariantList variant_list{};
        variant_list.Get(0).Set("OnFailedToEnterWorld");
        variant_list.Get(1).Set("1");

        uint32_t data_size;
        uint8_t *data = variant_list.SerializeToMem(&data_size, nullptr);

        GameUpdatePacket packet{};
        packet.packet_type = PACKET_CALL_FUNCTION;
        packet.net_id = -1;
        packet.flags |= 0x8;
        packet.data_extended_size = data_size;
        get_player()->send_raw_packet(NET_MESSAGE_GAME_PACKET, &packet, sizeof(GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

        delete[] data;
    }
}
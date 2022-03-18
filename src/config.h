#pragma once
#include <string>

namespace config {
    constexpr auto debug = true;
    constexpr std::string_view address = "127.0.0.1";

    namespace server_gateway {
        constexpr auto count = 2;
        constexpr auto port = 17256;
        constexpr auto max_peer = 32;
    }

    namespace server_game {
        constexpr auto count = 4;
        constexpr auto port = server_gateway::port - (server_game::count + 1);
        constexpr auto max_peer = 32;
    }

    namespace cdn {
        constexpr std::string_view host = "ubistatic-a.akamaihd.net";
        constexpr std::string_view cache = "0098/32621/cache/";
    }

    namespace on_super_main {
        constexpr auto param2 = cdn::host;
        constexpr auto param3 = cdn::cache;
        constexpr std::string_view param4 = "cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster";
        constexpr std::string_view param5 = "proto=158|choosemusic=audio/mp3/about_theme.mp3|active_holiday=2|wing_week_day=0|ubi_week_day=0|server_tick=83430103|clash_active=1|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|";
    }

    namespace data {
        constexpr std::string_view root = "./data/";
        constexpr std::string_view player = "./data/player/";
        constexpr std::string_view world = "./data/world/";
    }
}
#include <spdlog/spdlog.h>

#include "generictextevent.h"
#include "player.h"
#include "../config.h"
#include "../items/itemsdb.h"
#include "../utils/textparse.h"
#include "../world/worldpool.h"

namespace player {
    void GenericTextEvent::quit() {
        enet_peer_disconnect_later(get_player()->get_peer(), 0);
    }

    void GenericTextEvent::guest() {
        /*TextParse text_parse{ get_player()->get_last_packet_text() };
        size_t user_hash{ std::hash<std::string>{}(text_parse.get("requestedName", 1)) };
        user_hash += get_player()->get_peer()->host->address.port;

        if (text_parse.get<uint32_t>("user", 1) != static_cast<uint32_t>(user_hash)) {
            enet_peer_disconnect_later(get_player()->get_peer(), 0);
            return;
        }*/

        get_player()->send_packet(NET_MESSAGE_CLIENT_LOG_REQUEST, "");

        get_player()->send_variant({
             "OnOverrideGDPRFromServer",
             18, // Age
             1,
             0,
             1
        }, 0);

        static std::string param2 = std::string{ config::on_super_main::param2 };
        static std::string param3 = std::string{ config::on_super_main::param3 };
        static std::string param4 = std::string{ config::on_super_main::param4 };
        static std::string param5 = std::string{ config::on_super_main::param5 };

        get_player()->send_variant({
           "OnSuperMainStartAcceptLogonHrdxs47254722215a",
           items::get_items_db()->get_hash(),
           param2,
           param3,
           param4,
           param5,
           -1 // This is player tribute data ok.
        });

        get_player()->send_packet(NET_MESSAGE_TRACK, "eventName|102_PLAYER.AUTHENTICATION\nAuthenticated|1\nAuthentication_error|0\nDevice_Id|^^30729b74-7b07-445e-a066-05c9f092d242\nGrow_Id|24395319\nName|^^ztz\nWorldlock_balance|0\neventType|0");
    }

    void GenericTextEvent::refresh_item_data() {
        get_player()->send_log("One moment, updating item data...", true);

        GameUpdatePacket game_update_packet{};
        game_update_packet.packet_type = PACKET_SEND_ITEM_DATABASE_DATA;
        game_update_packet.net_id = -1;
        game_update_packet.flags |= 0x8;
        game_update_packet.unk8 = items::get_items_db()->get_size();
        game_update_packet.data_extended_size = items::get_items_db()->get_compressed_size();
        get_player()->send_raw_packet(NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(GameUpdatePacket) - 4, items::get_items_db()->get_compressed_data(), ENET_PACKET_FLAG_RELIABLE);
    }

    void GenericTextEvent::enter_game() {
        get_player()->send_variant({
            "OnEmoticonDataChanged",
            0,
            "(wl)|ā|0&(yes)|Ă|0&(no)|ă|0&(love)|Ą|0&(oops)|ą|0&(shy)|Ć|0&(wink)|ć|0&(tongue)|Ĉ|0&(agree)|ĉ|0&(sleep)|Ċ|0&(punch)|ċ|0&(music)|Č|0&(build)|č|0&(megaphone)|Ď|0&(sigh)|ď|0&(mad)|Đ|0&(wow)|đ|0&(dance)|Ē|0&(see-no-evil)|ē|0&(bheart)|Ĕ|0&(heart)|ĕ|0&(grow)|Ė|0&(gems)|ė|0&(kiss)|Ę|0&(gtoken)|ę|0&(lol)|Ě|0&(smile)|Ā|0&(cool)|Ĝ|0&(cry)|ĝ|0&(vend)|Ğ|0&(bunny)|ě|0&(cactus)|ğ|0&(pine)|Ĥ|0&(peace)|ģ|0&(terror)|ġ|0&(troll)|Ġ|0&(evil)|Ģ|0&(fireworks)|Ħ|0&(football)|ĥ|0&(alien)|ħ|0&(party)|Ĩ|0&(pizza)|ĩ|0&(clap)|Ī|0&(song)|ī|0&(ghost)|Ĭ|0&(nuke)|ĭ|0&(halo)|Į|0&(turkey)|į|0&(gift)|İ|0&(cake)|ı|0&(heartarrow)|Ĳ|0&(lucky)|ĳ|0&(shamrock)|Ĵ|0&(grin)|ĵ|0&(ill)|Ķ|0&(eyes)|ķ|0&(weary)|ĸ|0&"
        }); 
        
        get_player()->send_variant({
            "OnRequestWorldSelectMenu",
            "add_filter|\nadd_floater|ANJAYPITERP|0|0.5|2147418367"
        });

        get_player()->send_log(fmt::format("Where would you like to go? (`w{}`` others online)", get_player()->get_peer()->host->connectedPeers));
    }

    void GenericTextEvent::join_request() {
        world::get_world_pool()->join_request(get_player(), "dek1");
    }
}
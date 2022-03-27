#include "requestedName.h"
#include "../../../config.h"
#include "../../../items/itemsdb.h"
#include "../../../utils/textparse.h"
#include "../../../utils/random.h"

namespace events {
    requestedName::requestedName(player::Player *player) {
        player->load_once("requestedName", [player](const std::string &string) {
            if (string.empty()) {
                enet_peer_disconnect_later(player->get_peer(), 0);
                return;
            }

            TextParse text_parse{ string };
            player->set_raw_name(text_parse.get("requestedName", 1));
            player->set_mac(text_parse.get("mac", 1));
            player->set_country(text_parse.get("country", 1));
            player->set_display_name("`w" + player->get_raw_name() + "_" + std::to_string(utils::random::get_generator_static().uniform(100, 999)));

            if (!player->load_()) {
                player->set_user_id(player->save(-1, true));
            }

            static std::string param2 = std::string{ config::on_super_main::param2 };
            static std::string param3 = std::string{ config::on_super_main::param3 };
            static std::string param4 = std::string{ config::on_super_main::param4 };
            static std::string param5 = std::string{ config::on_super_main::param5 };

            player->send_variant({
                "OnSuperMainStartAcceptLogonHrdxs47254722215a",
                items::get_items_db()->get_hash(),
                param2,
                param3,
                param4,
                param5/*,
                -1*/ // This is player tribute data ok.
            });
        });
    }
}
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
            size_t user_hash{ std::hash<std::string>{}(text_parse.get("requestedName", 1)) };
            user_hash += player->get_peer()->host->address.port;

            if (text_parse.get<uint32_t>("user", 1) != static_cast<uint32_t>(user_hash)) {
                enet_peer_disconnect_later(player->get_peer(), 0);
                return;
            }

            uint64_t min = 4294967295;
            uint64_t max = min * 2;
            player->m_user_id = random::get_generator_static().uniform(min, max);
            player->m_raw_name = text_parse.get("requestedName", 1);
            player->m_display_name = "`w" + player->m_raw_name + "_" + std::to_string(random::get_generator_static().uniform(100, 999));
            player->m_country = text_parse.get("country", 1);

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
#include "player.h"

namespace player {
    Player::Player(ENetPeer* peer) : Peer{ peer }, m_net_id{ 0 } {}

    int Player::send_log(const std::string &log, bool on_console_message) {
        if (!on_console_message) {
            return send_packet(NET_MESSAGE_GAME_MESSAGE, fmt::format("action|log\nmsg|{}", log));
        }

        return send_variant({ "OnConsoleMessage", log });
    }
}
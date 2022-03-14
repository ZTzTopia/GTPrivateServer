#pragma once
#include <cstdint>
#include <memory>
#include <enet/enet.h>

namespace player {
    enum eNetMessageType {
        NET_MESSAGE_UNKNOWN = 0,
        NET_MESSAGE_SERVER_HELLO,
        NET_MESSAGE_GENERIC_TEXT,
        NET_MESSAGE_GAME_MESSAGE,
        NET_MESSAGE_GAME_PACKET,
        NET_MESSAGE_ERROR,
        NET_MESSAGE_TRACK,
        NET_MESSAGE_CLIENT_LOG_REQUEST,
        NET_MESSAGE_CLIENT_LOG_RESPONSE,
        NET_MESSAGE_GENERIC_TEXT2 = 0x69,
        NET_MESSAGE_GENERIC_TEXT3 = 0x48,
        NET_MESSAGE_CLIENT_LOG_GETASYNC = 0x24,
        NET_MESSAGE_CLIENT_TRACK_RESPONSE = 0x83,
        NET_MESSAGE_CLIENT_SYSTEM_RESPONSE = 0x44
    };

    enum ePacketType {
        PACKET_STATE = 0,
        PACKET_CALL_FUNCTION,
        PACKET_UPDATE_STATUS,
        PACKET_TILE_CHANGE_REQUEST,
        PACKET_SEND_MAP_DATA,
        PACKET_SEND_TILE_UPDATE_DATA,
        PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE,
        PACKET_TILE_ACTIVATE_REQUEST,
        PACKET_TILE_APPLY_DAMAGE,
        PACKET_SEND_INVENTORY_STATE,
        PACKET_ITEM_ACTIVATE_REQUEST,
        PACKET_ITEM_ACTIVATE_OBJECT_REQUEST,
        PACKET_SEND_TILE_TREE_STATE,
        PACKET_MODIFY_ITEM_INVENTORY,
        PACKET_ITEM_CHANGE_OBJECT,
        PACKET_SEND_LOCK,
        PACKET_SEND_ITEM_DATABASE_DATA,
        PACKET_SEND_PARTICLE_EFFECT,
        PACKET_SET_ICON_STATE,
        PACKET_ITEM_EFFECT,
        PACKET_SET_CHARACTER_STATE,
        PACKET_PING_REPLY,
        PACKET_PING_REQUEST,
        PACKET_GOT_PUNCHED,
        PACKET_APP_CHECK_RESPONSE,
        PACKET_APP_INTEGRITY_FAIL,
        PACKET_DISCONNECT,
        PACKET_BATTLE_JOIN,
        PACKET_BATTLE_EVENT,
        PACKET_USE_DOOR,
        PACKET_SEND_PARENTAL,
        PACKET_GONE_FISHIN,
        PACKET_STEAM,
        PACKET_PET_BATTLE,
        PACKET_NPC,
        PACKET_SPECIAL,
        PACKET_SEND_PARTICLE_EFFECT_V2,
        PACKET_ACTIVE_ARROW_TO_ITEM,
        PACKET_SELECT_TILE_INDEX,
        PACKET_SEND_PLAYER_TRIBUTE_DATA,
        PACKET_PVE_UNK1,
        PACKET_PVE_UNK2,
        PACKET_PVE_UNK3,
        PACKET_PVE_UNK4,
        PACKET_PVE_UNK5,
        PACKET_SET_EXTRA_MODS,
        PACKET_ON_STEP_ON_TILE_MOD,
        PACKET_MAXVAL
    };

    struct GameUpdatePacket {
        ePacketType packet_type; // 0
        uint32_t unk4; // 4
        uint32_t unk5; // 8
        uint32_t unk6; // 12
        uint32_t unk7; // 16
        uint32_t unk8; // 20
        float unk9; // 24
        float unk10; // 28
        float unk11; // 32
        float unk12; // 36
        float unk13; // 40
        uint32_t unk14; // 44
        uint32_t unk15; // 48
        int32_t data_extended_length; // 52
        uint32_t data_extended; // 56
    };

    inline eNetMessageType get_message_type(ENetPacket *packet) {
        if (packet->dataLength > 3) {
            return static_cast<eNetMessageType>(*packet->data);
        }

        return NET_MESSAGE_UNKNOWN;
    }

    inline char *get_text(ENetPacket *packet){
        std::memset(packet->data + packet->dataLength - 1, 0, 1);
        return (char*)(packet->data + 4);
    }

    inline char *get_struct(ENetPacket *packet, int length);
    inline GameUpdatePacket *get_struct(ENetPacket *packet);
    inline uint8_t *get_extended_data(GameUpdatePacket *gameUpdatePacket);
}

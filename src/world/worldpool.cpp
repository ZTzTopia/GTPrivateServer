#include "worldpool.h"

namespace world {
    WorldPool::~WorldPool() {
        for (auto &world : m_worlds) {
            delete world;
        }

        m_worlds.clear();
    }

    struct InventoryItem {
        __int16 itemID;
        __int8 itemCount;
    };

    struct PlayerInventory {
        std::vector<InventoryItem> items;
        int inventorySize = 100;
    };

    void sendInventory(ENetPeer* peer, PlayerInventory inventory)
    {
        int inventoryLen = inventory.items.size();
        int packetLen = 66 + (inventoryLen * 4) + 4;
        BYTE* data2 = new BYTE[packetLen];
        int MessageType = 0x4;
        int PacketType = 0x9;
        int NetID = -1;
        int CharState = 0x8;

        memset(data2, 0, packetLen);
        memcpy(data2, &MessageType, 4);
        memcpy(data2 + 4, &PacketType, 4);
        memcpy(data2 + 8, &NetID, 4);
        memcpy(data2 + 16, &CharState, 4);
        int endianInvVal = _byteswap_ulong(inventoryLen);
        memcpy(data2 + 66 - 4, &endianInvVal, 4);
        endianInvVal = _byteswap_ulong(inventory.inventorySize);
        memcpy(data2 + 66 - 8, &endianInvVal, 4);
        int val = 0;
        for (int i = 0; i < inventoryLen; i++)
        {
            val = 0;
            val |= inventory.items.at(i).itemID;
            val |= inventory.items.at(i).itemCount << 16;
            val &= 0x00FFFFFF;
            val |= 0x00 << 24;
            memcpy(data2 + (i * 4) + 66, &val, 4);
        }
        ENetPacket* packet3 = enet_packet_create(data2,
                                                 packetLen,
                                                 ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet3);
        delete data2;
    }

    void WorldPool::join_request(player::Player *player, const std::string &world_name) {
        auto world = new World{ world_name };
        world->generate_new_world();
        add_world(world);

        player->set_current_world_name_hash(std::hash<std::string>{}(world_name));

        uint32_t data_size;
        uint8_t *data = world->serialize_to_mem(&data_size, nullptr);

        player::GameUpdatePacket game_update_packet{};
        game_update_packet.packet_type = player::PACKET_SEND_MAP_DATA;
        game_update_packet.net_id = -1;
        game_update_packet.flags |= 0x8;
        game_update_packet.data_extended_size = data_size;
        game_update_packet.data_extended = reinterpret_cast<uint32_t&>(data);
        player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, data, ENET_PACKET_FLAG_RELIABLE);

        player->send_variant({
            "OnSpawn",
            fmt::format(
                "spawn|avatar\n"
                "netID|1\n"
                "userID|123123\n"
                "colrect|0|0|20|30\n"
                "posXY|1250|64\n"
                "name|ztz\n"
                "country|us\n"
                "invis|0\n"
                "mstate|0\n"
                "smstate|1\n"
                "onlineID|\n"
                "type|local\n"
            )
        });

        PlayerInventory inventory;
        sendInventory(player->get_peer(), inventory);

        player->send_variant({
            "OnSetClothing",
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            123123,
            { 0, 0, 0 }
        }, 1);

        game_update_packet = {};
        game_update_packet.packet_type = player::PACKET_SET_CHARACTER_STATE;
        game_update_packet.unk1 = 0;
        game_update_packet.unk2 = 0x80;
        game_update_packet.unk3 = 0x80;
        game_update_packet.net_id = 1;
        game_update_packet.unk7 = 125.f;
        game_update_packet.unk8 = 0;
        game_update_packet.unk9 = 1200.f;
        game_update_packet.unk10 = 250.f;
        game_update_packet.unk11 = 250.f;
        game_update_packet.unk12 = 1000.f;
        player->send_raw_packet(player::NET_MESSAGE_GAME_PACKET, &game_update_packet, sizeof(player::GameUpdatePacket) - 4, nullptr);
    }

    void WorldPool::quit_to_exit(player::Player *player, bool disconnected) {
        auto *world = get_world(player->get_current_world_name_hash());
        if (!world) {
            return;
        }
    }

    void WorldPool::add_world(World *world) {
        m_worlds.emplace_back(world);
    }

    void WorldPool::remove_world(World *world) {
        delete world;
        m_worlds.erase(std::remove(m_worlds.begin(), m_worlds.end(), world), m_worlds.end());
        m_worlds.shrink_to_fit();
    }

    void WorldPool::remove_world(const std::string &world_name) {
        remove_world(get_world(world_name));
    }

    World *WorldPool::get_world(const std::string &world_name) {
        if (world_name.empty()) {
            return nullptr;
        }

        for (auto &world : m_worlds) {
            if (world->get_world_name_hash() == std::hash<std::string>{}(world_name)) {
                return world;
            }
        }

        return nullptr;
    }

    World *WorldPool::get_world(std::size_t world_name_hash) {
        for (auto &world : m_worlds) {
            if (world->get_world_name_hash() == world_name_hash) {
                return world;
            }
        }

        return nullptr;
    }

    std::vector<World *> WorldPool::get_worlds() {
        return m_worlds;
    }

    size_t WorldPool::get_world_count() {
        return m_worlds.size();
    }
}

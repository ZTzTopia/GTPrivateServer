#pragma once
#include <string>
#include <vector>

#include "tile.h"
#include "../player/player.h"
#include "../proton/shared/common.h"

namespace world {
    class World {
    public:
        explicit World(const std::string &name);
        ~World();

        void send_to_all(std::function<void(player::Player *)> callback);

        void add_player(player::Player *player);
        void remove_player(player::Player *player);
        void respawn_player(player::Player *player) {};

        void generate(uint16_t width = 100, uint16_t height = 54);
        void load(const std::string &name) {}; // Todo: load from file.

        Tile *get_tile(int x, int y);
        Tile *get_tile(CL_Vec2i vec2i) { return get_tile(vec2i.x, vec2i.y); }

        uint8_t *serialize_to_mem(uint32_t *size_out, uint8_t *dest);

        [[nodiscard]] std::string get_name() const { return m_name; }
        [[nodiscard]] std::vector<player::Player *> get_players() const { return m_players; }
        [[nodiscard]] CL_Vec2i get_white_door_pos() const { return m_white_door_pos; }
        [[nodiscard]] uint32_t increase_total_net_id() { return ++m_total_net_id; }
        [[nodiscard]] uint32_t get_total_net_id() const { return m_total_net_id; }

   private:
        std::string m_name;
        uint16_t m_width;
        uint16_t m_height;
        CL_Vec2i m_white_door_pos;

        std::vector<Tile *> m_tiles;
        std::vector<player::Player *> m_players;

        uint32_t m_total_net_id;
        uint32_t m_total_object_id;
        uint64_t m_owner_user_id;
    };
}

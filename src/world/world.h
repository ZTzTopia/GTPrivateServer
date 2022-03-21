#pragma once
#include <string>
#include <vector>

#include "tile.h"
#include "../player/player.h"
#include "../proton/shared/common.h"

namespace world {
    class World {
    public:
        explicit World(std::string name);
        ~World();

        void send_to_all(const std::function<void(player::Player *)> &callback);

        void add_player(player::Player *player);
        void remove_player(player::Player *player);
        void respawn_player(player::Player *player) {};

        void generate(uint16_t width = 100, uint16_t height = 54);

        void save(bool insert = false);
        bool load();

        Tile *get_tile(uint16_t x, uint16_t y);
        Tile *get_tile(CL_Vec2i vec2i) { return get_tile(vec2i.x, vec2i.y); }
        CL_Vec2i get_tile_pos(uint16_t id);
        CL_Vec2i get_tile_fg_pos(uint16_t id);
        CL_Vec2i get_tile_bg_pos(uint16_t id);

        uint8_t *serialize_to_mem(uint32_t *size_out, uint8_t *dest);

        [[nodiscard]] std::string get_name() const { return m_name; }
        [[nodiscard]] bool is_fresh_world() const { return m_tiles.empty(); }
        [[nodiscard]] std::vector<player::Player *> get_players() const { return m_players; }
        [[nodiscard]] uint32_t increase_total_net_id() { return ++m_total_net_id; }
        [[nodiscard]] uint32_t get_total_net_id() const { return m_total_net_id; }

   private:
        size_t m_last_tile_hash;

        std::string m_name;
        uint16_t m_width;
        uint16_t m_height;

        std::vector<Tile *> m_tiles;
        std::vector<player::Player *> m_players;

        uint32_t m_total_net_id;
        uint32_t m_total_object_id;
        uint64_t m_owner_user_id;
    };
}

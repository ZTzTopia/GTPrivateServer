#pragma once
#include <vector>

#include "world.h"
#include "../player/player.h"

namespace world {
    class WorldPool {
    public:
        WorldPool() = default;
        ~WorldPool();

        void join_request(player::Player *player, const std::string &world_name);
        void quit_to_exit(player::Player *player, bool disconnected = false);

        World *new_world(const std::string &world_name);
        void add_world(World *world);
        void remove_world(World *world);
        void remove_world(const std::string &world_name);

        World *get_world(const std::string &world_name);
        World *get_world(size_t world_name_hash);
        std::vector<World *> get_worlds();
        size_t get_world_count();

    private:
        std::vector<World *> m_worlds;
    };

    inline WorldPool *get_world_pool() {
        static auto *world_pool = new WorldPool{};
        return world_pool;
    }
}
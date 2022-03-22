#pragma once
#include <vector>

#include "world.h"
#include "../player/player.h"

namespace world {
    class WorldPool {
    public:
        WorldPool() = default;
        ~WorldPool();

        uint8_t *get_world_data(const std::string &world_name, uint32_t *data_size);

        World *new_world(const std::string &world_name);
        void add_world(World *world);
        void remove_world(World *world);
        void remove_world(const std::string &world_name);

        World *get_world(const std::string &world_name);
        World *get_world_or_generate(const std::string &world_name);
        bool is_world_loaded(const std::string &world_name);
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
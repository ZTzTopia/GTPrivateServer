#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "world.h"

namespace world {
    class WorldPool {
    public:
        WorldPool() = default;
        ~WorldPool();

        std::shared_ptr<World> new_world(const std::string& name);
        void remove_world(const std::string& name);

        std::shared_ptr<World> get_world(const std::string& name);

    public:
        std::size_t get_world_count() const { return m_worlds.size(); }

    private:
        std::unordered_map<std::string, std::shared_ptr<World>> m_worlds;
    };
}

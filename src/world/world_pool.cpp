#include "world_pool.h"

namespace world {
    WorldPool::~WorldPool()
    {
        for (auto& world : m_worlds) {
            world.second.reset();
        }

        m_worlds.clear();
    }

    std::shared_ptr<World> WorldPool::new_world(const std::string& name)
    {
        m_worlds[name] = std::make_shared<World>(name);
        m_worlds[name]->generate();
        return m_worlds[name];
    }

    void WorldPool::remove_world(const std::string& name)
    {
        m_worlds[name].reset();
        m_worlds.erase(name);
    }

    std::shared_ptr<World> WorldPool::get_world(const std::string& name)
    {
        for (auto& world : m_worlds) {
            if (world.first == name) {
                return world.second;
            }
        }

        return nullptr;
    }
}
#include "worldpool.h"

namespace world {
    WorldPool::~WorldPool() {
        for (auto &world : m_worlds) {
            delete world;
        }

        m_worlds.clear();
    }

    uint8_t *WorldPool::get_world_data(const std::string &world_name, uint32_t *data_size) {
        World *world = get_world(world_name);
        if (world == nullptr) {
             world = new_world(world_name);
        }

        uint8_t *data = world->serialize_to_mem(data_size, nullptr);
        return data;
    }

    World *WorldPool::new_world(const std::string &world_name) {
        auto world = new World{ world_name };
        if (!world->load(world_name)) {
            world->generate();
        }
        add_world(world);
        return world;
    }

    void WorldPool::add_world(World *world) {
        m_worlds.emplace_back(world);
    }

    void WorldPool::remove_world(World *world) {
        if (!world) {
            return;
        }

        m_worlds.erase(std::remove(m_worlds.begin(), m_worlds.end(), world), m_worlds.end());
        m_worlds.shrink_to_fit();

        delete world;
    }

    void WorldPool::remove_world(const std::string &world_name) {
        remove_world(get_world(world_name));
    }

    World *WorldPool::get_world(const std::string &world_name) {
        if (world_name.empty()) {
            return nullptr;
        }

        for (auto &world : m_worlds) {
            if (world->get_name() == world_name) {
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

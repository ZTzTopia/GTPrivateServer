#pragma once
#include <vector>

#include "iteminfo.h"

namespace items {
    class ItemsDB {
    public:
        ItemsDB();
        ~ItemsDB();

        void init();

        uint32_t get_hash() const { return m_hash; }
        uint8_t *get_data() const { return m_data; }
        uint32_t get_size() const { return m_data_size; }

    private:
        std::vector<ItemInfo> m_items;

        uint32_t m_item_count;
        uint16_t m_version;

        uint32_t m_hash;
        uint32_t m_data_size;
        uint8_t *m_data;
    };

    inline ItemsDB *get_items_db() {
        static auto *items_db = new ItemsDB{};
        return items_db;
    }
}
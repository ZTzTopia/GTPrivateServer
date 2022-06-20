#pragma once
#include <vector>

#include "item_info.h"

namespace item {
    class ItemDB {
    public:
        ItemDB();
        ~ItemDB();

        bool load();

    public:
        [[nodiscard]] std::size_t get_size() const { return m_items.size(); }
        [[nodiscard]] std::vector<uint8_t> get_data() const { return m_data; }
        [[nodiscard]] uint32_t get_hash() const { return m_hash; }

        [[nodiscard]] std::size_t get_compressed_size() const { return m_compressed_size; }
        [[nodiscard]] std::vector<uint8_t> get_compressed_data() const { return m_compressed_data; }

    private:
        std::size_t m_size;
        std::vector<uint8_t> m_data;
        uint32_t m_hash;

        std::size_t m_compressed_size;
        std::vector<uint8_t> m_compressed_data;

        uint16_t m_version;
        uint32_t m_item_count;
        std::vector<ItemInfo*> m_items;
    };
}

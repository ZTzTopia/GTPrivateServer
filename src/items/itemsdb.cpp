#include <fstream>
#include <spdlog/spdlog.h>

#include "itemsdb.h"
#include "../config.h"
#include "../../vendor/proton/shared/util/ResourceUtils.h"

namespace items {
    ItemsDB::ItemsDB()
        : m_item_count(0)
        , m_version(0)
        , m_hash(0)
        , m_data_size(-1)
        , m_data(nullptr)
        , m_compressed_data_size(-1)
        , m_compressed_data(nullptr) {}

    ItemsDB::~ItemsDB() {
        for (auto &item : m_items) {
            delete item;
        }

        m_items.clear();
        delete m_data;
        delete m_compressed_data;
    }

    uint32_t hash_string(const char *data, uint32_t length) {
        uint32_t hash = 0x55555555;
        if (data) {
            if (length >= 1) {
                while (length) {
                    hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t *>(data++);
                    length--;
                }
            }
            else {
                while (*data) {
                    hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const uint8_t*>(data++);
                }
            }
        }

        return hash;
    }

    bool ItemsDB::serialize() {
        spdlog::info("Loading items database..");

        std::ifstream file(fmt::format("{}/items.dat", config::data::root), std::ifstream::in | std::ifstream::binary);
        if (!file.is_open()) {
            return false;
        }

        file.seekg(0, std::ifstream::end);
        m_data_size = file.tellg();
        file.seekg(0, std::ifstream::beg);

        if (m_data_size <= 0) {
            return false;
        }

        m_data = new uint8_t[m_data_size];
        file.read(reinterpret_cast<char*>(m_data), m_data_size);
        file.close();

        m_hash = hash_string(reinterpret_cast<const char *>(m_data), m_data_size);
        m_compressed_data = zlibDeflateToMemory(m_data, static_cast<int>(m_data_size), reinterpret_cast<int *>(&m_compressed_data_size));

        std::memcpy(&m_version, m_data, 2);
        uint32_t mem_pos = 2;

        if (m_version < 11 || m_version > 14) {
            return false;
        }

        std::memcpy(&m_item_count, m_data + mem_pos, 4);
        mem_pos += 4;

        spdlog::info("Items database hash: {}, version: {}, item count: {}", m_hash, m_version, m_item_count);

        m_items.reserve(m_item_count);
        for (uint32_t i = 0; i < m_item_count; i++) {
            m_items.emplace_back(ItemInfo::serialize(m_data, m_version, mem_pos));
        }

        // We need the data??
        delete[] m_data;
        m_data = nullptr;
        return true;
    }
}
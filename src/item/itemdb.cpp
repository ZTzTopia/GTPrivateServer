#include <fstream>
#include <vector>
#include <spdlog/spdlog.h>
#include <util/ResourceUtils.h>

#include "itemdb.h"
#include "../utils/binary_reader.h"
#include "../utils/hash.h"

namespace item {
    ItemDB::ItemDB() : m_size{}, m_hash{} {}

    ItemDB::~ItemDB()
    {
        m_data.clear();
    }

    bool ItemDB::load()
    {
        std::ifstream file{ "./items.dat", std::ifstream::in | std::ifstream::binary };
        if (!file.is_open()) {
            return false;
        }

        m_size = file.seekg(0, std::ios::end).tellg();
        m_data.resize(m_size);

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(m_data.data()), static_cast<std::streamsize>(m_size));
        file.close();

        m_hash = utils::proton_hash(reinterpret_cast<const char*>(m_data.data()), static_cast<uint32_t>(m_size));

        int compressed_size{};
        uint8_t* compressed_data{ zlibDeflateToMemory(m_data.data(), static_cast<int>(m_size), &compressed_size) };

        m_compressed_size = compressed_size;
        m_compressed_data.resize(m_compressed_size);
        std::copy(compressed_data, compressed_data + m_compressed_size, m_compressed_data.data());

        BinaryReader binary_reader{ m_data.data() };
        m_version = binary_reader.read_u16();
        m_item_count = binary_reader.read_u32();
        m_items.reserve(m_item_count);

        std::size_t position = binary_reader.position();
        for (std::size_t i = 0; i < m_item_count; i++) {
            m_items.push_back(new ItemInfo{});
            m_items[i]->serialize(m_data.data(), m_version, position);

            if (m_items[i]->id != i) {
                spdlog::error("Item id mismatch: {} != {}", m_items[i]->id, i);
                break;
            }
        }

        return true;
    }
}

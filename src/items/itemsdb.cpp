#include <fstream>
#include <spdlog/spdlog.h>

#include "itemsdb.h"
#include "../config.h"
#include "../proton/shared/util/ResourceUtils.h"

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

    std::string cypher(const std::string &input, uint32_t id) {
        if (input.empty()) {
            return "";
        }

        constexpr std::string_view key = "PBG892FXX982ABC*";
        std::string return_value(input.size(), 0);

        for (uint32_t i = 0; i < input.size(); i++) {
            return_value[i] = static_cast<char>(input[i] ^ key[(i + id) % key.size()]);
        }

        return return_value;
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

    bool ItemsDB::initialize() {
        spdlog::info("Loading items database..");

        std::ifstream file(fmt::format("{}/items.dat", config::data::root), std::ifstream::in | std::ifstream::binary);
        if (!file.is_open()) {
            return false;
        }

        std::streamoff begin = file.tellg();
        file.seekg(0, std::ifstream::end);
        std::streamoff end = file.tellg();
        file.seekg(0, std::ifstream::beg);
        m_data_size = static_cast<uint32_t>(end - begin);

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

        spdlog::info("Items database hash: {}, version: {}", m_hash, m_version);

        std::memcpy(&m_item_count, m_data + mem_pos, 4);
        mem_pos += 4;

        m_items.reserve(m_item_count);
        for (uint32_t i = 0; i < m_item_count; i++) {
            auto *item_info = new ItemInfo{};

            std::memcpy(&item_info->id, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->editable_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->category, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->action_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->hit_sound_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->name_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->name = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->name_length);
            item_info->name = cypher(item_info->name, item_info->id);
            mem_pos += item_info->name_length;

            std::memcpy(&item_info->texture_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->texture = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->texture_length);
            mem_pos += item_info->texture_length;

            std::memcpy(&item_info->texture_hash, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->kind, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->flags, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->texture_pos_x, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->texture_pos_y, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->spread_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->stripey_wallpaper, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->collision_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->break_hits, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->reset_time, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->clothing_type, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->rarity, m_data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->max_amount, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->extra_file_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_file = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->extra_file_length);
            mem_pos += item_info->extra_file_length;

            std::memcpy(&item_info->extra_file_hash, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->audio_volume, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->pet_name_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_name = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->pet_name_length);
            mem_pos += item_info->pet_name_length;

            std::memcpy(&item_info->pet_prefix_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_prefix = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->pet_prefix_length);
            mem_pos += item_info->pet_prefix_length;

            std::memcpy(&item_info->pet_suffix_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_suffix = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->pet_suffix_length);
            mem_pos += item_info->pet_suffix_length;

            std::memcpy(&item_info->pet_ability_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_ability = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->pet_ability_length);
            mem_pos += item_info->pet_ability_length;

            std::memcpy(&item_info->seed_base, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->seed_overlay, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->tree_base, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->tree_leaves, m_data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->seed_color, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_overlay_color, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_ingredient, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_grow_time, m_data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->flags2, m_data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->is_rayman, m_data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->extra_option_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_option = std::string(reinterpret_cast<char*>(m_data + mem_pos), item_info->extra_option_length);
            mem_pos += item_info->extra_option_length;

            std::memcpy(&item_info->texture2_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->texture2 = std::string(reinterpret_cast<char*>(m_data + mem_pos), item_info->texture2_length);
            mem_pos += item_info->texture2_length;

            std::memcpy(&item_info->extra_option2_length, m_data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_option2 = std::string(reinterpret_cast<char*>(m_data + mem_pos), item_info->extra_option2_length);
            mem_pos += item_info->extra_option2_length;

            std::memcpy(&item_info->pad, m_data + mem_pos, 80);
            mem_pos += 80;

            if (m_version >= 11) {
                std::memcpy(&item_info->punch_option_length, m_data + mem_pos, 2);
                mem_pos += 2;

                item_info->punch_option = std::string(reinterpret_cast<char *>(m_data + mem_pos), item_info->punch_option_length);
                mem_pos += item_info->punch_option_length;
            }

            if (m_version >= 12) {
                std::memcpy(&item_info->flags3, m_data + mem_pos, 4);
                mem_pos += 4;

                std::memcpy(&item_info->body_part, m_data + mem_pos, 9);
                mem_pos += 9;
            }

            if (m_version >= 13) {
                std::memcpy(&item_info->flags4, m_data + mem_pos, 4);
                mem_pos += 4;
            }

            if (m_version >= 14) {
                std::memcpy(&item_info->flags5, m_data + mem_pos, 4);
                mem_pos += 4;
            }

            m_items.emplace_back(item_info);
        }

        // We need the data??
        delete[] m_data;
        m_data = nullptr;
        return true;
    }
}
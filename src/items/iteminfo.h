#pragma once
#include <cstdint>
#include <string>
#include <cstring>

namespace items {
    struct ItemInfo {
        uint32_t id;
        uint8_t editable_type;
        uint8_t category;
        uint8_t action_type;
        uint8_t hit_sound_type;
        uint16_t name_length;
        std::string name;
        uint16_t texture_length;
        std::string texture;
        uint32_t texture_hash;
        uint8_t kind;
        uint32_t flags;
        uint8_t texture_pos_x;
        uint8_t texture_pos_y;
        uint8_t spread_type;
        uint8_t stripey_wallpaper;
        uint8_t collision_type;
        uint8_t break_hits;
        uint32_t reset_time;
        uint8_t clothing_type;
        uint16_t rarity;
        uint8_t max_amount;
        uint16_t extra_file_length;
        std::string extra_file;
        uint32_t extra_file_hash;
        uint32_t audio_volume;
        uint16_t pet_name_length;
        std::string pet_name;
        uint16_t pet_prefix_length;
        std::string pet_prefix;
        uint16_t pet_suffix_length;
        std::string pet_suffix;
        uint16_t pet_ability_length;
        std::string pet_ability;
        uint8_t seed_base;
        uint8_t seed_overlay;
        uint8_t tree_base;
        uint8_t tree_leaves;
        uint32_t seed_color;
        uint32_t seed_overlay_color;
        uint32_t seed_ingredient;
        uint32_t seed_grow_time;
        uint16_t flags2;
        uint16_t is_rayman;
        uint16_t extra_option_length;
        std::string extra_option;
        uint16_t texture2_length;
        std::string texture2;
        uint16_t extra_option2_length;
        std::string extra_option2;
        uint8_t pad[80];
        uint16_t punch_option_length;
        std::string punch_option;
        uint32_t flags3;
        uint8_t body_part[9];
        uint32_t flags4;
        uint32_t flags5;

        static std::string cypher(const std::string &input, uint32_t id) {
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

        static ItemInfo *serialize(uint8_t *data, uint16_t version, uint32_t &mem_pos) {
            auto *item_info = new ItemInfo{};

            std::memcpy(&item_info->id, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->editable_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->category, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->action_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->hit_sound_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->name_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->name = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->name_length);
            item_info->name = cypher(item_info->name, item_info->id);
            mem_pos += item_info->name_length;

            std::memcpy(&item_info->texture_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->texture = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->texture_length);
            mem_pos += item_info->texture_length;

            std::memcpy(&item_info->texture_hash, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->kind, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->flags, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->texture_pos_x, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->texture_pos_y, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->spread_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->stripey_wallpaper, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->collision_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->break_hits, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->reset_time, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->clothing_type, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->rarity, data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->max_amount, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->extra_file_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_file = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->extra_file_length);
            mem_pos += item_info->extra_file_length;

            std::memcpy(&item_info->extra_file_hash, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->audio_volume, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->pet_name_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_name = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->pet_name_length);
            mem_pos += item_info->pet_name_length;

            std::memcpy(&item_info->pet_prefix_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_prefix = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->pet_prefix_length);
            mem_pos += item_info->pet_prefix_length;

            std::memcpy(&item_info->pet_suffix_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_suffix = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->pet_suffix_length);
            mem_pos += item_info->pet_suffix_length;

            std::memcpy(&item_info->pet_ability_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->pet_ability = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->pet_ability_length);
            mem_pos += item_info->pet_ability_length;

            std::memcpy(&item_info->seed_base, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->seed_overlay, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->tree_base, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->tree_leaves, data + mem_pos, 1);
            mem_pos += 1;

            std::memcpy(&item_info->seed_color, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_overlay_color, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_ingredient, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->seed_grow_time, data + mem_pos, 4);
            mem_pos += 4;

            std::memcpy(&item_info->flags2, data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->is_rayman, data + mem_pos, 2);
            mem_pos += 2;

            std::memcpy(&item_info->extra_option_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_option = std::string(reinterpret_cast<char*>(data + mem_pos), item_info->extra_option_length);
            mem_pos += item_info->extra_option_length;

            std::memcpy(&item_info->texture2_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->texture2 = std::string(reinterpret_cast<char*>(data + mem_pos), item_info->texture2_length);
            mem_pos += item_info->texture2_length;

            std::memcpy(&item_info->extra_option2_length, data + mem_pos, 2);
            mem_pos += 2;

            item_info->extra_option2 = std::string(reinterpret_cast<char*>(data + mem_pos), item_info->extra_option2_length);
            mem_pos += item_info->extra_option2_length;

            std::memcpy(&item_info->pad, data + mem_pos, 80);
            mem_pos += 80;

            if (version >= 11) {
                std::memcpy(&item_info->punch_option_length, data + mem_pos, 2);
                mem_pos += 2;

                item_info->punch_option = std::string(reinterpret_cast<char *>(data + mem_pos), item_info->punch_option_length);
                mem_pos += item_info->punch_option_length;
            }

            if (version >= 12) {
                std::memcpy(&item_info->flags3, data + mem_pos, 4);
                mem_pos += 4;

                std::memcpy(&item_info->body_part, data + mem_pos, 9);
                mem_pos += 9;
            }

            if (version >= 13) {
                std::memcpy(&item_info->flags4, data + mem_pos, 4);
                mem_pos += 4;
            }

            if (version >= 14) {
                std::memcpy(&item_info->flags5, data + mem_pos, 4);
                mem_pos += 4;
            }

            return item_info;
        }
    };
}
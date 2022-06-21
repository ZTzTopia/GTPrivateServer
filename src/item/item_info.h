#pragma once
#include "../utils/binary_reader.h"

namespace item {
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

        static std::string cypher(const std::string& input, uint32_t id_);
        void serialize(void* buffer, uint16_t version, std::size_t& position);
    };
}
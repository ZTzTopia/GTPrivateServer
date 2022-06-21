#include "item_info.h"

namespace item {
    std::string ItemInfo::cypher(const std::string& input, uint32_t id_)
    {
        constexpr std::string_view key{ "PBG892FXX982ABC*" };
        std::string return_value(input.size(), 0);

        for (uint32_t i = 0; i < input.size(); i++)
            return_value[i] = static_cast<char>(input[i] ^ key[(i + id_) % key.size()]);

        return return_value;
    }

    void ItemInfo::serialize(void* buffer, uint16_t version, std::size_t& position)
    {
        BinaryReader binary_reader{ buffer };
        binary_reader.skip(position);

        id = binary_reader.read_u32();
        editable_type = binary_reader.read_u8();
        category = binary_reader.read_u8();
        action_type = binary_reader.read_u8();
        hit_sound_type = binary_reader.read_u8();
        name = cypher(binary_reader.read_string(), id);
        texture = binary_reader.read_string();
        texture_hash = binary_reader.read_u32();
        kind = binary_reader.read_u8();
        flags = binary_reader.read_u32();
        texture_pos_x = binary_reader.read_u8();
        texture_pos_y = binary_reader.read_u8();
        spread_type = binary_reader.read_u8();
        stripey_wallpaper = binary_reader.read_u8();
        collision_type = binary_reader.read_u8();
        break_hits = binary_reader.read_u8();
        reset_time = binary_reader.read_u32();
        clothing_type = binary_reader.read_u8();
        rarity = binary_reader.read_u16();
        max_amount = binary_reader.read_u8();
        extra_file = binary_reader.read_string();
        extra_file_hash = binary_reader.read_u32();
        audio_volume = binary_reader.read_u32();
        pet_name = binary_reader.read_string();
        pet_prefix = binary_reader.read_string();
        pet_suffix = binary_reader.read_string();
        pet_ability = binary_reader.read_string();
        seed_base = binary_reader.read_u8();
        seed_overlay = binary_reader.read_u8();
        tree_base = binary_reader.read_u8();
        tree_leaves = binary_reader.read_u8();
        seed_color = binary_reader.read_u32();
        seed_overlay_color = binary_reader.read_u32();
        seed_ingredient = binary_reader.read_u32();
        seed_grow_time = binary_reader.read_u32();
        flags2 = binary_reader.read_u16();
        is_rayman = binary_reader.read_u16();
        extra_option = binary_reader.read_string();
        texture2 = binary_reader.read_string();
        extra_option2 = binary_reader.read_string();
        binary_reader.skip(80);

        if (version > 10) {
            punch_option = binary_reader.read_string();
        }

        if (version > 11) {
            flags3 = binary_reader.read_u32();
            binary_reader.skip(9); // Body part.
        }

        if (version > 12) {
            binary_reader.skip(4); // unk
        }

        if (version > 13) {
            binary_reader.skip(4); // unk
        }

        position = binary_reader.position();
    }
}

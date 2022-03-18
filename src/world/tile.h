#pragma once
#include <cstdint>
#include <chrono>

#include "../items/iteminfo.h"
#include "../items/itemsdb.h"

namespace world {
    enum eTileFlag {
        TILEFLAG_NONE = 0,
        TILEFLAG_TILEEXTRA = 1 << 0,
        TILEFLAG_LOCKED = 1 << 1,
        TILEFLAG_SEED = 1 << 4,
        TILEFLAG_FLIPPED = 1 << 5,
        TILEFLAG_OPEN = 1 << 6,
        TILEFLAG_PUBLIC = 1 << 7,
        TILEFLAG_SILENCED = 1 << 9,
        TILEFLAG_WATER = 1 << 10,
        TILEFLAG_FIRE = 1 << 12,
        TILEFLAG_RED = 1 << 13,
        TILEFLAG_BLUE = 1 << 14,
        TILEFLAG_GREEN = 1 << 15,
    };

    class Tile {
    public:
        Tile();
        ~Tile() = default;

        [[nodiscard]] uint8_t get_hit_count();
        void reset_hit_count_by_time();
        void reset_hit_count();

        uint32_t calculate_memory_needed() const;

        void add_tile(uint16_t fg, uint16_t bg);
        void remove_tile();

        void set_fg(uint16_t fg) { m_fg = fg; }
        [[nodiscard]] uint16_t get_fg() const { return m_fg; }

        void set_bg(uint16_t bg) { m_bg = bg; }
        [[nodiscard]] uint16_t get_bg() const { return m_bg; }

        void set_flags(uint16_t flags) { m_flags |= flags; }
        void remove_flag(uint16_t flags) { m_flags &= ~flags; }
        [[nodiscard]] uint16_t get_flags() const { return m_flags; }

        void set_label(const std::string &label) { set_flags(TILEFLAG_TILEEXTRA); m_label = label; }
        [[nodiscard]] std::string get_label() const { return m_label; }

        [[nodiscard]] uint16_t get_front_tile() const { return m_fg != 0 ? m_fg : m_bg; }
        [[nodiscard]] items::ItemInfo *get_item_info() const { return items::get_items_db()->get_item_info(get_front_tile()); }

    private:
        uint16_t m_fg;
        uint16_t m_bg;
        uint16_t m_flags;

        uint8_t m_hit_count;
        std::chrono::high_resolution_clock::time_point m_last_hit;

        std::string m_label;
    };
}
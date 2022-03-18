#include "tile.h"

namespace world {
    Tile::Tile()
        : m_fg(0)
        , m_bg(0)
        , m_flags(TILEFLAG_NONE)
        , m_hit_count(0) {}

    uint32_t Tile::calculate_memory_needed() const {
        uint32_t mem_need{ 8 };
        if ((m_flags & TILEFLAG_TILEEXTRA) == 1) {
            mem_need += 4;
        }

        return mem_need;
    }

    uint8_t Tile::get_hit_count() {
        m_last_hit = std::chrono::high_resolution_clock::now();
        return ++m_hit_count;
    }

    void Tile::reset_hit_count_by_time() {
        if (m_last_hit.time_since_epoch().count() == 0) {
            return;
        }

        auto diff = std::chrono::high_resolution_clock::now() - m_last_hit;
        if (std::chrono::duration_cast<std::chrono::seconds>(diff).count() >= get_item_info()->reset_time) {
            reset_hit_count();
        }
    }

    void Tile::reset_hit_count() {
        m_hit_count = 0;
        m_last_hit = {};
    }

    void Tile::add_tile(uint16_t fg, uint16_t bg) {
        m_fg = fg;
        m_bg = bg;
    }

    void Tile::remove_tile() {
        if (m_fg != 0) {
            m_fg = 0;
        }
        else {
            m_bg = 0;
        }

        reset_hit_count();
        m_flags = TILEFLAG_NONE;
    }
}
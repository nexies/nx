#pragma once

#include <cstdint>
#include <nx/common/types/enum_flags.hpp>

namespace nx::tui {

// ── Key codes ─────────────────────────────────────────────────────────────────

enum class key : uint16_t {
    none = 0,
    printable,      // character field holds the codepoint

    // Whitespace / editing
    enter,
    tab,
    escape,
    backspace,
    delete_key,
    insert,

    // Navigation
    home,
    end,
    page_up,
    page_down,

    // Arrow keys
    arrow_up,
    arrow_down,
    arrow_left,
    arrow_right,

    // Function keys
    f1,  f2,  f3,  f4,
    f5,  f6,  f7,  f8,
    f9,  f10, f11, f12,
};

// ── Modifier flags ────────────────────────────────────────────────────────────

enum class key_modifier : uint8_t {
    none  = 0x00,
    shift = 0x01,
    alt   = 0x02,
    ctrl  = 0x04,
    meta  = 0x08,
};

NX_FLAGS(key_modifier, key_modifiers)

} // namespace nx::tui

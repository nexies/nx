#pragma once

#include <nx/tui/input/key.hpp>
#include <nx/core2/event/event.hpp>

namespace nx::tui {

struct  key_event : public nx::core::event
{
    static constexpr nx::core::event::type_id type_press   = 0x0101;
    static constexpr nx::core::event::type_id type_release = 0x0102;

    key           code      = key::none;
    key_modifiers modifiers;
    char32_t      character = 0; // set when code == key::printable

    explicit key_event(key           c  = key::none,
                       key_modifiers m  = {},
                       char32_t      ch = 0,
                       nx::core::event::type_id t = type_press) noexcept
        : nx::core::event(t), code(c), modifiers(m), character(ch)
    {}
};

} // namespace nx::tui

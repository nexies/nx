#pragma once

#include <nx/tui/input/key.hpp>
#include <nx/tui/types/point.hpp>

namespace nx::tui {

enum class mouse_button : uint8_t {
    none,
    left,
    middle,
    right,
    wheel_up,
    wheel_down,
};

enum class mouse_action : uint8_t {
    press,
    release,
    move,
};

struct mouse_event
{
    mouse_button  button    = mouse_button::none;
    mouse_action  action    = mouse_action::move;
    key_modifiers modifiers;
    point<int>    position  = { 0, 0 }; // col, row (1-based, as reported by terminal)
};

} // namespace nx::tui

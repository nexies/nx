#pragma once

#include <nx/tui/input/key.hpp>
#include <nx/tui/types/point.hpp>
#include <nx/core2/event/event.hpp>

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
    wheel
};

struct mouse_event : public nx::core::event
{
    static constexpr nx::core::event::type_id TYPE = 0x0110;

    mouse_button  button    = mouse_button::none;
    mouse_action  action    = mouse_action::move;
    key_modifiers modifiers;
    point<int>    position  = { 0, 0 }; // col, row (1-based, as reported by terminal)

    mouse_event() noexcept : nx::core::event(TYPE) {}
};

} // namespace nx::tui

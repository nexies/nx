#pragma once

#include <variant>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>

namespace nx::tui {

using input_event = std::variant<key_event, mouse_event>;

} // namespace nx::tui

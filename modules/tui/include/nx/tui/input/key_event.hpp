#pragma once

#include <nx/tui/input/key.hpp>

namespace nx::tui {

struct key_event
{
    key           code      = key::none;
    key_modifiers modifiers;
    char32_t      character = 0; // set when code == key::printable
};

} // namespace nx::tui

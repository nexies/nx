#pragma once

#include <nx/tui/terminal/terminal.hpp>

namespace nx::tui {

    class mouse_guard
    {
    public:
        mouse_guard()  { terminal::enable_mouse_tracking(); }
        ~mouse_guard() { terminal::disable_mouse_tracking(); }

        mouse_guard(const mouse_guard &) = delete;
        mouse_guard & operator=(const mouse_guard &) = delete;
    };

} // namespace nx::tui

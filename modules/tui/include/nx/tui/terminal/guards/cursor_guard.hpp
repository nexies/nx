#pragma once

#include <nx/tui/terminal/terminal.hpp>

namespace nx::tui {

    class cursor_guard
    {
    public:
        explicit cursor_guard(bool visible = false)
        {
            terminal::set_cursor_visible(visible);
        }

        ~cursor_guard()
        {
            terminal::set_cursor_visible(true);
        }

        cursor_guard(const cursor_guard &) = delete;
        cursor_guard & operator=(const cursor_guard &) = delete;
    };

} // namespace nx::tui

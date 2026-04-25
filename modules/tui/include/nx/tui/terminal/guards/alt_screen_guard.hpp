#pragma once

#include <nx/tui/terminal/terminal.hpp>

namespace nx::tui {

    class alt_screen_guard
    {
    public:
        alt_screen_guard()
        {
            terminal::save_cursor();
            terminal::enable_alt_buffer();
            terminal::erase_screen();
            terminal::move_cursor_home();
        }

        ~alt_screen_guard()
        {
            terminal::erase_screen();
            terminal::disable_alt_buffer();
            terminal::restore_cursor();
        }

        alt_screen_guard(const alt_screen_guard &) = delete;
        alt_screen_guard & operator=(const alt_screen_guard &) = delete;
    };

} // namespace nx::tui

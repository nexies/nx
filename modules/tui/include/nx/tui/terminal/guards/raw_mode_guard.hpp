#pragma once

#include <nx/tui/terminal/terminal.hpp>

namespace nx::tui {

    class raw_mode_guard
    {
    public:
        raw_mode_guard()  { terminal::enable_raw_mode(); }
        ~raw_mode_guard() { terminal::disable_raw_mode(); }

        raw_mode_guard(const raw_mode_guard &) = delete;
        raw_mode_guard & operator=(const raw_mode_guard &) = delete;
    };

} // namespace nx::tui

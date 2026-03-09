//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TERMINAL_HPP
#define NX_TERMINAL_HPP

#include <nx/core.hpp>
#include <nx/tui/types/Size.hpp>

namespace nx::tui
{
    class Terminal
    {
    public:
        static WindowSize GetWindowSize();
    };
}

#endif //NX_TERMINAL_HPP
//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TERMINAL_HPP
#define NX_TERMINAL_HPP

#include <nx/core.hpp>
#include <nx/tui/types/size.hpp>
#include <nx/tui/types/color.hpp>

namespace nx::tui
{
    class Terminal
    {
    public:
        static WindowSize
        GetWindowSize();

        static ColorType
        GetColorSupport();

        static void
        SetColorSupport(ColorType type);
    };
}

#endif //NX_TERMINAL_HPP
//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TERMINAL_HPP
#define NX_TERMINAL_HPP

#include <nx/core.hpp>

namespace nx::tui
{
    struct WindowSize
    {
        small_size_t columns;
        small_size_t rows;

        small_size_t pixelWidth;
        small_size_t pixelHeight;
    };

    class Terminal
    {
    public:
        static WindowSize getWindowSize();
    };
}

#endif //NX_TERMINAL_HPP
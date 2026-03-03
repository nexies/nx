//
// Created by nexie on 03.03.2026.
//

#include <sys/ioctl.h>
#include <unistd.h>

#include <nx/tui/Terminal.hpp>

using namespace nx::tui;

WindowSize Terminal::getWindowSize()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, w.ws_row, w.ws_xpixel, w.ws_ypixel};
}
//
// Created by nexie on 03.03.2026.
//

#include <sys/ioctl.h>
#include <unistd.h>

#include <../include/nx/tui/terminal/Terminal.hpp>

using namespace nx::tui;

WindowSize Terminal::GetWindowSize()
{
    winsize w { 0 };
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_row, w.ws_col, w.ws_xpixel, w.ws_ypixel};
}
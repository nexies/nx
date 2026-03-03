//
// Created by nexie on 03.03.2026.
//

#include <nx/tui/App.hpp>

using namespace nx::tui;

void App::_onSIGWINCH()
{
    auto ws = Terminal::getWindowSize();
    NX_EMIT(windowChanged, ws);
}

void App::_onSignalFromOS(int signal)
{
    if (signal == SIGWINCH)
        return _onSIGWINCH();

    nx::App::_onSignalFromOS(signal);
}

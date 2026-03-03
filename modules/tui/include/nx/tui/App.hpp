//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TUI_APP_HPP
#define NX_TUI_APP_HPP

#include <nx/core.hpp>
#include <nx/tui/Terminal.hpp>

namespace nx::tui
{
    class App : public ::nx::App
    {
    public:
        NX_SIGNAL(windowChanged, WindowSize);

    protected:
        void _onSIGWINCH();
        void _onSignalFromOS(int signal) override;
    };
}

#endif //NX_TUI_APP_HPP

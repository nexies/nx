//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TUI_APP_HPP
#define NX_TUI_APP_HPP

#include <nx/core.hpp>
#include <nx/tui/terminal/Terminal.hpp>

namespace nx::tui
{
    class Application : public ::nx::core::Application
    {
    public:
        NX_SIGNAL(windowChanged, WindowSize);

        Application();
        Application(int argc, char * argv[]);

        static Application * Instance ();

    protected:
        void _onSIGWINCH();
        void _onSIGNAL(int signal) override;
    };

# define nxApp (::nx::tui::Application::Instance())
}

#endif //NX_TUI_APP_HPP

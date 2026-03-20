//
// Created by nexie on 03.03.2026.
//

#include <nx/tui/App.hpp>

using namespace nx::tui;

void Application::_onSIGWINCH()
{
    auto ws = Terminal::GetWindowSize();
    NX_EMIT(windowChanged, ws);
}

void Application::_onSIGNAL(int signal)
{
    nxDebug("Received signal: {}", signal);

    if (signal == SIGWINCH)
        return _onSIGWINCH();

    nx::core::Application::_onSIGNAL(signal);
}

Application::Application() :
    nx::core::Application()
{}

Application::Application(int argc, char *argv []) :
    nx::core::Application(argc, argv)
{}

Application * Application::Instance() {
    return dynamic_cast<nx::tui::Application *>(nx::core::Application::Instance());
}

nx::Result Application::_beforeExec()
{
    Terminal::SaveCursor();
    Terminal::EnableAltBuffer();
    Terminal::EraseScreen();
    Terminal::MoveCursorHome();
    Terminal::SetCursorVisible(false);

    return core::Application::_beforeExec();
    // return Result::Ok();
}

nx::Result Application::_afterExec()
{
    Terminal::SetCursorVisible(true);
    Terminal::EraseScreen();
    Terminal::DisableAltBuffer();
    Terminal::RestoreCursor();

    // return Result::Ok();
    return core::Application::_afterExec();
}

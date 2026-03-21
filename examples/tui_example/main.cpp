//
// Created by nexie on 03.03.2026.
//

#include <iostream>
#include <nx/tui/App.hpp>

#include <nx/tui/types/color.hpp>

#include "nx/tui/terminal/Screen.hpp"
#include "nx/tui/graphics/painter.hpp"

struct WinSizeRecv : public nx::Object
{
    void onWinSizeChange(nx::tui::WindowSize ws)
    {
        nxDebug("Window size changed -- rows[{}], columns[{}], height[{}], width[{}]",
            ws.chars.height, ws.chars.width, ws.pixels.height, ws.pixels.width);

    }
};


int main (int argc, char * argv[])
{
    using namespace nx::tui;
    Application app;

    WinSizeRecv wsr;

    nx::connect(&app, &Application::windowChanged,
        &wsr, &WinSizeRecv::onWinSizeChange);


    Screen screen;

    auto p = screen.getBuffer().getPainter();
    p.setColor(Color::YellowLight);
    p.setStyle( Underlined  );

    p.drawText({20, 10}, "Hello, Ayana!!! :)");


    std::string text = "*";
    // Point point ((screen.getBuffer().size().width - text.size())/2, screen.getBuffer().size().height/2);
    auto rect = screen.getBuffer().rect();

    // for (int i = 0; i < 300; i++)
        // std::cerr << ((i+1) % 10);
    // std::cerr << std::endl;

    // std::cerr << rect.width() << std::endl;

    // p.drawText(rect.topLeft(), text);
    // p.drawText(rect.topRight(), text);
    // p.drawText(rect.bottomLeft(), text);
    // p.drawText(rect.bottomLeft(), text);

    screen.render();

    return app.exec();


    // Terminal::SetColorSupport(ColorType::TrueColor);

    // for (int i = 0; i < 64; i++)
    // {
    //     float f = i / 64.0f;
    //     std::cout
    //     << Color::Interpolate(f, Color::Blue, Color::Yellow).print(true)
    //     << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
    //     << Color::Default
    //     << std::endl << std::flush;
    // }
    // std::cout << Color::Default;

    // std::cout << ansi::EraseScreen << std::endl;


    // Terminal::MoveCursorHome();

    // fprintf(stdout, "%s", ansi::EnableAltBuffer);
    // Terminal::SaveCursor();
    //
    // Terminal::EnableAltBuffer();
    // Terminal::EraseScreen();
    // Terminal::MoveCursorHome();
    // Terminal::SetCursorVisible(false);
    // // fprintf(stdout, "%s", ansi::SaveScreen);
    // Terminal::EnableUnderline(true);
    // Terminal::EnableDim(true);
    //
    // auto w = Terminal::GetWindowSize();
    // Terminal::MoveCursor(w.chars.height / 2, w.chars.width / 2);
    //
    // fprintf(stdout, "Screen saved");
    // Terminal::EnableUnderline(false);
    // Terminal::EnableDim(false);
    // std::cin.get();
    // // fprintf(stdout, "%s", ansi::RestoreScreen);
    // fprintf(stdout, "Screen restored");
    // Terminal::SetCursorVisible(true);
    // Terminal::EraseScreen();
    // Terminal::DisableAltBuffer();
    // Terminal::RestoreCursor();

    // fprintf(stdout, "%s", ansi::DisableAltBuffer);
    return 0;
}
//
// Created by nexie on 03.03.2026.
//

#include <iostream>
#include <nx/tui/App.hpp>

#include <nx/tui/types/color.hpp>

struct WinSizeRecv : public nx::Object
{
    void onWinSizeChange(nx::tui::WindowSize ws)
    {
        nxDebug("Window size changed -- rows[{}], columns[{}], height[{}], width[{}]",
            ws.chars.height, ws.chars.width, ws.pixels.height, ws.pixels.width);

    }
};

#include <nx/tui/terminal/ansi_codes.p.hpp>

int main (int argc, char * argv[])
{
    using namespace nx::tui;

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


    fprintf(stdout, "%s", ansi::EnableAltBuffer);
    // fprintf(stdout, "%s", ansi::SaveScreen);
    fprintf(stdout, "Screen saved");
    std::cin.get();
    // fprintf(stdout, "%s", ansi::RestoreScreen);
    fprintf(stdout, "Screen restored");
    fprintf(stdout, "%s", ansi::DisableAltBuffer);

    return 0;
}
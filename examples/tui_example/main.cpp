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

int main (int argc, char * argv[])
{
    using namespace nx::tui;

    Terminal::SetColorSupport(ColorType::TrueColor);

    for (int i = 0; i < 64; i++)
    {
        float f = i / 64.0f;
        std::cout << Color::Interpolate(f, Color::Blue, Color::Negative(Color::Red)).print(true) << "   f = " << f << std::endl;
    }
    std::cout << Color::Default;

    return 0;
}
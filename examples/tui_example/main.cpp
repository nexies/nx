//
// Created by nexie on 03.03.2026.
//

#include <nx/tui/App.hpp>

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

    Application a(argc, argv);
    WinSizeRecv wsrecv;

    nx::connect(&a, &Application::windowChanged,
        &wsrecv, &WinSizeRecv::onWinSizeChange);


    return a.exec();
}
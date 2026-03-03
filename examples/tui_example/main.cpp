//
// Created by nexie on 03.03.2026.
//

#include <nx/tui/App.hpp>

struct WinSizeRecv : public nx::Object
{
    void onWinSizeChange(nx::tui::WindowSize ws)
    {
        nxDebug("Window size changed -- rows[{}], columns[{}], height[{}], width[{}]",
            ws.rows, ws.columns, ws.pixelHeight, ws.pixelWidth);

    }
};

int main (int argc, char * argv[])
{
    nx::tui::App::Init(argc, argv);
    WinSizeRecv wsrecv;

    return nx::tui::App::Exec();
}
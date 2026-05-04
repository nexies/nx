//
// Created by nexie on 5/4/2026.
//

#include <nx/tui/application.hpp>

#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/frame.hpp>

#include <nx/tui/layout/box.hpp>


int main(int argc, char ** argv)
{
    using namespace nx::tui;

    tui_application app(argc, argv);

    auto src = app.main_screen();
    auto fr = new frame(src);
    fr->set_title("Line edit example");
    fr->set_focus_policy(widget::focus_policy::no_focus);

    auto vbox = new v_box(fr);
    vbox->set_spacing(1);
    vbox->set_margin(3);

    auto line = new line_edit(vbox);

    auto butt = new button(vbox);
    butt->set_text("Click me");

    return app.exec();
}

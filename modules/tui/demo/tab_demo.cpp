// tab_demo — demonstrates tab_widget with animated underline indicator.
//
// Keys:
//   Left / Right arrow (while tab_bar focused) — switch tabs
//   Mouse click on a tab label                 — switch tabs
//   Tab / Shift+Tab                            — cycle widget focus
//   Escape                                     — quit

#include <fmt/format.h>

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/tab_widget.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/layout/box.hpp>

using namespace nx::tui;

int main()
{
    tui_application app;
    app.set_theme(theme::dark());

    auto * scr  = app.main_screen();
    auto * root = new v_box(scr);
    root->set_spacing(0);

    // ── Header ────────────────────────────────────────────────────────────────

    auto * hdr = new frame(root);
    hdr->set_border_style(border_style::single);
    hdr->set_title("tab_widget demo");
    hdr->set_fixed_height(3);
    auto * hdr_lbl = new label(hdr);
    hdr_lbl->set_text("Left/Right = switch tab    Tab = focus    Esc = quit");

    // ── tab_widget ────────────────────────────────────────────────────────────

    auto * tabs = new tab_widget(root);

    // Page 1 — a few buttons
    auto * page1 = new v_box(tabs);
    page1->set_spacing(1);
    page1->set_margin({1, 3, 1, 3});
    auto * lbl1 = new label(page1);
    lbl1->set_text("This is the first tab.");
    auto * btn1 = new button(page1);
    btn1->set_text("Button A");
    btn1->set_fixed_height(3);
    auto * btn2 = new button(page1);
    btn2->set_text("Button B");
    btn2->set_fixed_height(3);
    tabs->add_tab(page1, "General");

    // Page 2 — a text input
    auto * page2 = new v_box(tabs);
    page2->set_spacing(1);
    page2->set_margin(1);
    auto * lbl2 = new label(page2);
    lbl2->set_text("Enter some text below:");
    auto * edit = new line_edit(page2);
    edit->set_fixed_height(1);
    tabs->add_tab(page2, "Input");

    // Page 3 — plain text
    auto * page3 = new v_box(tabs);
    page3->set_margin(1);
    for (int i = 1; i <= 8; ++i) {
        auto * l = new label(page3);
        l->set_text(fmt::format("  About line {:d}", i));
    }
    tabs->add_tab(page3, "About");

    return app.exec();
}

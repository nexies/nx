#include <fmt/format.h>

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/menu.hpp>
#include <nx/tui/widgets/menu_item.hpp>
#include <nx/tui/widgets/separator.hpp>
#include <nx/tui/layout/box.hpp>
#include <nx/tui/types/theme.hpp>

int main()
{
    using namespace nx::tui;

    tui_application app;
    app.set_theme(theme::dark());

    auto * scr  = app.main_screen();
    auto * root = new h_box(scr);
    root->set_spacing(1);
    root->set_margin(2);

    // ── Left — menu ───────────────────────────────────────────────────────────

    auto * menu_frame = new frame(root);
    menu_frame->set_title("Themes");
    menu_frame->set_border_style(border_style::rounded);

    auto * m = new menu(menu_frame);

    const char * entries[] = {
        "Dark (Catppuccin Mocha)",
        "Light (Catppuccin Latte)",
        "Halloween",
    };
    for (const char * e : entries)
        m->add_entry(e);

    m->set_current(0);

    // ── Right — info panel ────────────────────────────────────────────────────

    auto * right = new v_box(root);
    right->set_spacing(1);

    auto * info_frame = new frame(right);
    info_frame->set_title("Selection");
    info_frame->set_border_style(border_style::rounded);

    auto * info_box = new v_box(info_frame);
    info_box->set_spacing(0);
    info_box->set_margin(1);

    auto * lbl_current = new label(info_box);
    lbl_current->set_text("Current:  Dark (Catppuccin Mocha)");

    auto * lbl_status = new label(info_box);
    lbl_status->set_text("Activated: —");

    auto * sep = new separator(right);
    sep->set_orientation(orientation::horizontal);
    sep->set_fixed_height(1);

    auto * btn_apply = new button(right);
    btn_apply->set_text("Apply theme");

    // ── Signals ───────────────────────────────────────────────────────────────

    nx::core::connect(m, &menu::current_changed, lbl_current,
        [m, lbl_current](int i) {
            auto * item = m->item_at(i);
            lbl_current->set_text(fmt::format("Current:  {}", item ? item->text() : "—"));
        });

    nx::core::connect(m, &menu::activated, lbl_status,
        [m, lbl_status](int i) {
            auto * item = m->item_at(i);
            lbl_status->set_text(fmt::format("Activated: {}", item ? item->text() : "—"));
        });

    nx::core::connect(btn_apply, &button::clicked, m,
        [&app, m]() {
            switch (m->current()) {
            case 0: app.set_theme(theme::dark());       break;
            case 1: app.set_theme(theme::light());      break;
            case 2: app.set_theme(theme::halloween());  break;
            default: break;
            }
            app.main_screen()->update();
        });

    return app.exec();
}

#include <fmt/format.h>

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/separator.hpp>
#include <nx/tui/widgets/slider.hpp>
#include <nx/tui/widgets/spin_box.hpp>
#include <nx/tui/widgets/combo_box.hpp>
#include <nx/tui/widgets/text_edit.hpp>
#include <nx/tui/layout/box.hpp>
#include <nx/tui/types/theme.hpp>

int main()
{
    using namespace nx::tui;

    tui_application app;
    app.set_theme(theme::dark());

    auto * scr  = app.main_screen();
    auto * root = new h_box(scr);
    root->set_spacing(2);
    root->set_margin(2);

    // ── Left column ───────────────────────────────────────────────────────────

    auto * left = new v_box(root);
    left->set_spacing(1);
    left->set_fixed_width(34);
    left->set_horizontal_policy(size_policy::fixed);

    // ── Slider section ────────────────────────────────────────────────────────

    auto * sl_frame = new frame(left);
    sl_frame->set_title("slider");
    sl_frame->set_border_style(border_style::rounded);

    auto * sl_box = new v_box(sl_frame);
    sl_box->set_spacing(1);
    sl_box->set_margin(1);

    auto * lbl_vol = new label(sl_box);
    lbl_vol->set_text("Volume: 50");

    auto * sl = new slider(sl_box);
    sl->set_range(0, 100);
    sl->set_value(50);

    nx::core::connect(sl, &slider::value_changed, lbl_vol,
        [lbl_vol](int v) { lbl_vol->set_text(fmt::format("Volume: {}", v)); });

    // ── Spin box section ──────────────────────────────────────────────────────

    auto * sp_frame = new frame(left);
    sp_frame->set_title("spin_box");
    sp_frame->set_border_style(border_style::rounded);

    auto * sp_box = new v_box(sp_frame);
    sp_box->set_spacing(1);
    sp_box->set_margin(1);

    auto * lbl_spin = new label(sp_box);
    lbl_spin->set_text("Count: 0");

    auto * sp = new spin_box(sp_box);
    sp->set_range(-99, 99);
    sp->set_value(0);
    sp->set_suffix(" items");

    nx::core::connect(sp, &spin_box::value_changed, lbl_spin,
        [lbl_spin](int v) { lbl_spin->set_text(fmt::format("Count: {}", v)); });

    // ── Combo box section ─────────────────────────────────────────────────────

    auto * cb_frame = new frame(left);
    cb_frame->set_title("combo_box");
    cb_frame->set_border_style(border_style::rounded);

    auto * cb_box = new v_box(cb_frame);
    cb_box->set_spacing(1);
    cb_box->set_margin(1);

    auto * lbl_cb = new label(cb_box);
    lbl_cb->set_text("Theme: Dark");

    auto * cb = new combo_box(cb_box);
    cb->add_item("Dark (Catppuccin Mocha)");
    cb->add_item("Light (Catppuccin Latte)");
    cb->add_item("Halloween");

    nx::core::connect(cb, &combo_box::activated, lbl_cb,
        [&app, cb, lbl_cb](int i) {
            lbl_cb->set_text(fmt::format("Theme: {}", cb->item_text(i)));
            switch (i) {
            case 0: app.set_theme(theme::dark());      break;
            case 1: app.set_theme(theme::light());     break;
            case 2: app.set_theme(theme::halloween()); break;
            }
            app.main_screen()->update();
        });

    // ── Right column — text_edit ──────────────────────────────────────────────

    auto * right = new v_box(root);
    right->set_spacing(0);

    auto * te_frame = new frame(right);
    te_frame->set_title("text_edit");
    te_frame->set_border_style(border_style::rounded);

    auto * lbl_cursor = new label(right);
    lbl_cursor->set_text("Ln 1, Col 1");
    lbl_cursor->set_fixed_height(1);
    lbl_cursor->set_vertical_policy(size_policy::fixed);

    auto * te = new text_edit(te_frame);
    te->set_text("Hello, world!\n\nType here...\nUTF-8: привет 🌍");

    nx::core::connect(te, &text_edit::cursor_moved, lbl_cursor,
        [lbl_cursor](int row, int col) {
            lbl_cursor->set_text(fmt::format("Ln {}, Col {}", row + 1, col + 1));
        });

    return app.exec();
}

// tier1_demo — showcases all Tier-1 widgets:
//   separator, check_box, radio_button/radio_group, progress_bar, spinner
//
// Keys:
//   Tab / Shift+Tab   — cycle focus
//   Space / Enter     — toggle focused check_box / radio_button
//   +  /  -           — increase / decrease progress bar value (when focused)
//   S                 — start/stop spinner (global)
//   Escape            — quit

#include <nx/tui/application.hpp>
#include <nx/tui/input/event_filter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/key.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/separator.hpp>
#include <nx/tui/widgets/check_box.hpp>
#include <nx/tui/widgets/radio_button.hpp>
#include <nx/tui/widgets/progress_bar.hpp>
#include <nx/tui/widgets/spinner.hpp>
#include <nx/tui/layout/box.hpp>

using namespace nx::tui;

// ── Global key filter ────────────────────────────────────────────────────────��

class global_keys : public event_filter {
    spinner      * spin_;
    progress_bar * bar_;
    float          bar_val_ = 0.0f;

public:
    global_keys(spinner * s, progress_bar * b) : spin_(s), bar_(b) {}

    bool filter_key(key_event & e) override
    {
        if (e.code != key::printable) return false;
        if (e.character == U's' || e.character == U'S') {
            spin_->is_spinning() ? spin_->stop() : spin_->start();
            return true;
        }
        if (e.character == U'+' || e.character == U'=') {
            bar_val_ = std::min(1.0f, bar_val_ + 0.1f);
            bar_->set_value(bar_val_);
            return true;
        }
        if (e.character == U'-' || e.character == U'_') {
            bar_val_ = std::max(0.0f, bar_val_ - 0.1f);
            bar_->set_value(bar_val_);
            return true;
        }
        if (e.character == U'q' || e.character == U'Q') {
            nx_tui_app->quit();
            return true;
        }
        if (e.code == key::escape) {
            nx_tui_app->quit();
            return true;
        }
        if (e.modifiers.has(key_modifier::ctrl) &&
            e.character == U'C') {
            nx_tui_app->quit();
            return true;
            }

        return false;
    }
};

// ── Main ──────────────────────────────────────────────────────────────────────

int main()
{
    tui_application app;
    app.set_theme(theme::dark());

    auto * scr  = app.main_screen();
    auto * root = new v_box(scr);
    root->set_spacing(0);

    // Header
    auto * hdr = new frame(root);
    hdr->set_border_style(border_style::single);
    hdr->set_title("Tier-1 widgets demo");
    hdr->set_fixed_height(3);
    auto * hdr_lbl = new label(hdr);
    hdr_lbl->set_text("Tab=focus  Space=toggle  +/-=bar  S=spinner  Esc=quit");

    // Main row
    auto * main_row = new h_box(root);
    main_row->set_spacing(1);

    // ── Left column ───────────────────────────────────────────────────────────

    auto * left = new v_box(main_row);
    left->set_spacing(1);
    left->set_fixed_width(36);
    left->set_horizontal_policy(size_policy::fixed);

    // check_box section
    auto * chk_frame = new frame(left);
    chk_frame->set_title("check_box");
    chk_frame->set_border_style(border_style::rounded);
    chk_frame->set_fixed_height(8);
    auto * chk_box = new v_box(chk_frame);
    chk_box->set_spacing(1);
    auto * cb1 = new check_box(chk_box);  cb1->set_text("Enable notifications");
    auto * cb2 = new check_box(chk_box);  cb2->set_text("Dark mode");  cb2->set_checked(true);
    auto * cb3 = new check_box(chk_box);  cb3->set_text("Auto-save");

    // radio_button section
    auto * rdo_frame = new frame(left);
    rdo_frame->set_title("radio_button");
    rdo_frame->set_border_style(border_style::rounded);
    rdo_frame->set_fixed_height(9);
    auto * rdo_box = new v_box(rdo_frame);
    rdo_box->set_spacing(1);
    auto * rb1 = new radio_button(rdo_box);  rb1->set_text("Option A");
    auto * rb2 = new radio_button(rdo_box);  rb2->set_text("Option B");
    auto * rb3 = new radio_button(rdo_box);  rb3->set_text("Option C");

    auto * grp = new radio_group(scr);
    grp->add(rb1);
    grp->add(rb2);
    grp->add(rb3);

    // ── Right column ──────────────────────────────────────────────────────────

    auto * right = new v_box(main_row);
    right->set_spacing(1);

    // progress_bar section
    auto * pb_frame = new frame(right);
    pb_frame->set_title("progress_bar  (+/- to change)");
    pb_frame->set_border_style(border_style::rounded);
    pb_frame->set_fixed_height(5);
    auto * pb_box = new v_box(pb_frame);
    pb_box->set_spacing(1);
    auto * bar = new progress_bar(pb_box);
    auto * bar2 = new progress_bar(pb_box);
    bar2->set_value(0.65f, false);
    bar2->set_show_text(false);

    // separator + spinner section
    auto * spin_frame = new frame(right);
    spin_frame->set_title("spinner  (S to start/stop)");
    spin_frame->set_border_style(border_style::rounded);
    spin_frame->set_fixed_height(7);
    auto * spin_box = new v_box(spin_frame);
    spin_box->set_spacing(1);

    auto * sep1 = new separator(spin_box);
    (void)sep1;
    auto * spin1 = new spinner(spin_box);
    spin1->set_label("Processing…");
    auto * spin2 = new spinner(spin_box);
    spin2->set_label("Uploading…");
    auto * sep2 = new separator(spin_box);
    (void)sep2;
    auto * info_lbl = new label(spin_box);
    info_lbl->set_text("  Press S to toggle");

    // ── Wire up ───────────────────────────────────────────────────────────────

    auto gk = global_keys(spin1, bar);
    app.install_event_filter(&gk);

    nx::core::connect(cb1, &check_box::toggled, &app, [spin2](bool c) {
        c ? spin2->start() : spin2->stop();
    });

    return app.exec();
}

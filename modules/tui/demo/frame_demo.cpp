// frame_demo — showcase border styles, titles, and nested frame content.
//
// Widget tree:
//
//   screen (v_box_layout, spacing=1)
//   ├── title_lbl              label       fixed h=1   ← h_gradient fg
//   ├── styles_row             widget      h_box_layout, spacing=2
//   │   ├── single_frame       frame(single)    ↓ v_box_layout
//   │   │   └── label                           ← h_gradient fg red→yellow
//   │   ├── double_frame       frame(double_)
//   │   │   └── label                           ← h_gradient fg blue→cyan
//   │   ├── rounded_frame      frame(rounded)
//   │   │   └── label                           ← h_gradient fg green→cyan_bright
//   │   ├── thick_frame        frame(thick)
//   │   │   └── label                           ← h_gradient fg magenta→red
//   │   └── dashed_frame       frame(dashed)
//   │       └── label                           ← h_gradient fg yellow→white
//   └── form_frame             frame(rounded, title="Edit")  ↓ v_box_layout
//       ├── name_row            widget   h_box_layout
//       │   ├── name_lbl        label    fixed w=10
//       │   └── name_edit       line_edit
//       └── btn_row             widget   h_box_layout
//           ├── ok_btn          button   fixed w=10  ← h_gradient bg green→cyan
//           └── spacer          widget   expanding
//
// Tab   — cycle focus
// Enter — confirm (logs to title label)
// Escape — quit

#include <nx/tui/application.hpp>
#include <nx/tui/layouts/layout.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>

using namespace nx::tui;

// ── frame_controller ──────────────────────────────────────────────────────────

class frame_controller : public nx::core::object {
    label     * title_lbl_;
    line_edit * name_edit_;
    screen    * scr_;

public:
    NX_OBJECT(frame_controller)

    frame_controller(label * title_lbl, line_edit * name_edit, screen * scr,
                     nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , title_lbl_(title_lbl), name_edit_(name_edit), scr_(scr)
    {}

    void on_ok() {
        const auto name = name_edit_->text();
        title_lbl_->set_text(name.empty()
            ? "  (no name entered)"
            : "  Hello, " + name + "!");
        scr_->render();
    }
};

// ── helpers ───────────────────────────────────────────────────────────────────

static label * make_style_frame(border_style s, const char * name,
                                nx::core::object * parent)
{
    auto * f = new frame(parent);
    f->set_border_style(s);
    f->set_title(name);
    f->set_fixed_height(5);
    f->set_content_layout(std::make_unique<v_box_layout>());

    auto * lbl = new label(f);
    lbl->set_text(name);
    lbl->set_alignment(text_align::center);
    return lbl;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto * scr = app.main_screen();
    scr->set_style(bg(color::black) | fg(color::white));

    auto root_vbox = std::make_unique<v_box_layout>();
    root_vbox->set_spacing(1);
    scr->set_layout(std::move(root_vbox));

    // ── Title ─────────────────────────────────────────────────────────────────

    auto * title_lbl = new label(scr);
    title_lbl->set_text("  nx::tui  Frame Demo  —  Tab: focus  Escape: quit LOOOooooooOOOOOOOooooOOOOoooOOOoooOOoooOOooolg stirng");
    title_lbl->set_style(h_gradient_bg(color::cyan_bright, color::black)
                       | h_gradient_fg(color::black,       color::white));

    // ── Border style showcase row ─────────────────────────────────────────────

    auto * styles_row = new widget(scr);
    styles_row->set_fixed_height(5);
    {
        auto hbox = std::make_unique<h_box_layout>();
        hbox->set_spacing(1);
        styles_row->set_layout(std::move(hbox));
    }

    make_style_frame(border_style::single,  "Single",  styles_row)->set_style(h_gradient_fg(color::red,     color::black));
    make_style_frame(border_style::double_, "Double",  styles_row)->set_style(h_gradient_fg(color::blue,    color::black));
    make_style_frame(border_style::rounded, "Rounded", styles_row)->set_style(h_gradient_fg(color::green,   color::black));
    make_style_frame(border_style::thick,   "Thick",   styles_row)->set_style(h_gradient_fg(color::magenta, color::black));
    make_style_frame(border_style::dashed,  "Dashed",  styles_row)->set_style(h_gradient_fg(color::yellow,  color::black));

    // ── Interactive form inside a rounded frame ───────────────────────────────

    auto * form_frame = new frame(scr);
    form_frame->set_border_style(border_style::rounded);
    form_frame->set_title("Enter your name");
    form_frame->set_border_color(color::cyan);
    {
        auto vbox = std::make_unique<v_box_layout>();
        vbox->set_spacing(1);
        form_frame->set_content_layout(std::move(vbox));
    }

    // Name row.
    auto * name_row = new widget(form_frame);
    name_row->set_fixed_height(1);
    name_row->set_layout(std::make_unique<h_box_layout>());

    auto * name_lbl = new label(name_row);
    name_lbl->set_text("Name:");
    name_lbl->set_fixed_width(10);

    auto * name_edit = new line_edit(name_row);
    name_edit->set_style(fg(color::white) | bg(color::gray_dark));

    // Button row.
    auto * btn_row = new widget(form_frame);
    btn_row->set_fixed_height(1);
    btn_row->set_layout(std::make_unique<h_box_layout>());

    auto * ok_btn = new button(btn_row);
    ok_btn->set_text("OK");
    ok_btn->set_fixed_width(10);
    ok_btn->set_style(fg(color::black) | h_gradient_bg(color::green, color::cyan));

    auto * spacer = new widget(btn_row);

    // ── Signals ───────────────────────────────────────────────────────────────

    frame_controller ctrl(title_lbl, name_edit, scr);

    nx::core::connect(ok_btn,    &button::clicked,
                      &ctrl,     &frame_controller::on_ok);
    nx::core::connect(name_edit, &line_edit::return_pressed,
                      &ctrl,     &frame_controller::on_ok);

    scr->set_focused_widget(name_edit);

    return app.exec();
}

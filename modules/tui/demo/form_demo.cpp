// form_demo — label, button, line_edit + Tab focus traversal.
//
// Widget tree (all sizing handled by box containers — no manual geometry):
//
//   screen
//   └── root (v_box, spacing=1)
//       ├── title_lbl         label      fixed h=1
//       ├── name_row          h_box      fixed h=1
//       │   ├── name_lbl      label      fixed w=10
//       │   └── name_edit     line_edit  expanding
//       ├── email_row         h_box      fixed h=1
//       │   ├── email_lbl     label      fixed w=10
//       │   └── email_edit    line_edit  expanding
//       ├── btn_row           h_box      fixed h=1, spacing=2
//       │   ├── submit_btn    button     fixed w=12
//       │   ├── clear_btn     button     fixed w=12
//       │   └── spacer        widget     expanding (fills rest)
//       ├── sep_line          separator_line  fixed h=1
//       └── log_view          log_view   expanding
//
// Tab / Shift+Tab   — cycle focus
// Enter on Submit   — log Name + Email
// Enter on Clear    — clear both fields
// Escape            — quit

#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include <fmt/format.h>

#include <nx/tui/application.hpp>
#include <nx/tui/input/input_reader.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/widgets/box.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/style_option.hpp>

using namespace nx::tui;

// ── log_view ──────────────────────────────────────────────────────────────────

class log_view : public widget {
    std::vector<std::string> lines_;

public:
    NX_OBJECT(log_view)

    explicit log_view(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::no_focus);
    }

    void append(std::string line) {
        lines_.push_back(std::move(line));
        update();
    }

    void clear_log() {
        lines_.clear();
        update();
    }

protected:
    void on_paint(painter & p) override {
        p.fill(" ");
        const int h = size().height;
        const int w = size().width;
        if (h <= 0 || w <= 0) return;

        const int start = std::max(0, static_cast<int>(lines_.size()) - h);
        for (int i = 0; i < h; ++i) {
            const int idx = start + i;
            if (idx < static_cast<int>(lines_.size())) {
                p.draw_text({0, i}, lines_[idx]);
            }
        }
    }
};

// ── separator_line ────────────────────────────────────────────────────────────

class separator_line : public widget {
public:
    NX_OBJECT(separator_line)

    explicit separator_line(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::no_focus);
        set_fixed_height(1);
    }

    size_type size_hint() const override { return { 1, size().width }; }

protected:
    void on_paint(painter & p) override {
        p.draw_text({0, 0}, std::string(size().width, '-'));
    }
};

// ── form_controller ───────────────────────────────────────────────────────────

class form_controller : public nx::core::object {
    line_edit * name_edit_;
    line_edit * email_edit_;
    screen    * scr_;
    log_view  * log_;

public:
    NX_OBJECT(form_controller)

    form_controller(line_edit * name_edit, line_edit * email_edit,
                    screen * scr, log_view * log,
                    nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , name_edit_(name_edit), email_edit_(email_edit)
        , scr_(scr), log_(log)
    {}

    void on_submit() {
        log_->append(fmt::format(
            "  Submitted — name: \"{}\"  email: \"{}\"",
            name_edit_->text(), email_edit_->text()));
        scr_->render();
    }

    void on_clear() {
        name_edit_ ->set_text("");
        email_edit_->set_text("");
        log_->append("  Cleared.");
        scr_->render();
    }

    void on_name_return() {
        scr_->set_focused_widget(email_edit_);
        scr_->render();
    }

    void on_email_return() {
        on_submit();
    }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto * scr = app.main_screen();
    scr->set_style(bg(color::rgb(5, 5, 5)));

    // ── Widget tree ───────────────────────────────────────────────────────────

    auto * root = new v_box(scr);
    root->set_spacing(1);

    // Title row.
    auto * title_lbl = new label(root);
    title_lbl->set_text("  nx::tui  Form Demo  —  Tab/Shift+Tab: focus  Escape: quit");
    title_lbl->set_style(fg(color::cyan_bright));


    // auto login_box = new widget(root);
    // login_box->set_fixed_width(40);

    // Name row.
    auto * name_row  = new h_box(root);
    name_row->set_fixed_height(1);
    name_row->set_spacing(2);
    // name_row->set_margin(6);

    auto * name_lbl  = new label(name_row);
    name_lbl->set_text("Name:");
    name_lbl->set_fixed_width(6);

    auto * name_edit = new line_edit(name_row);
    name_edit->set_style(fg(color::white) | bg(color::gray_dark));
    name_edit->set_fixed_width(20);
    // new widget(name_row);

    // Email row.
    auto * email_row  = new h_box(root);
    email_row->set_fixed_height(1);
    email_row->set_spacing(2);
    // email_row->set_margin(6);

    auto * email_lbl  = new label(email_row);
    email_lbl->set_text("Email:");
    email_lbl->set_fixed_width(6);

    auto * email_edit = new line_edit(email_row);
    email_edit->set_style(fg(color::white) | bg(color::gray_dark));
    email_edit->set_fixed_width(20);
    // new widget(email_row);

    // Button row.
    auto * btn_row = new h_box(root);
    btn_row->set_fixed_height(3);
    btn_row->set_spacing(2);
    // btn_row->set_margin(6);

    auto * submit_btn = new button(btn_row);
    submit_btn->set_text("Submit");
    submit_btn->set_fixed_width(12);
    // submit_btn->set_style(fg(color::black) | bg(color::green));

    // Spacer fills remaining width.
    new widget(btn_row);

    auto * clear_btn = new button(btn_row);
    clear_btn->set_text("Clear");
    clear_btn->set_fixed_width(12);
    // clear_btn->set_style(fg(color::black) | bg(color::yellow));


    // Separator.
    auto * sep = new separator_line(root);
    sep->set_style(fg(color::gray_dark));

    // Log view.
    auto * log = new log_view(root);
    log->append("  Ready. Fill in the fields and press Submit.");

    // ── Focus + signals ───────────────────────────────────────────────────────

    scr->set_focused_widget(name_edit);

    form_controller ctrl(name_edit, email_edit, scr, log);

    nx::core::connect(submit_btn, &button::clicked,
                      &ctrl, &form_controller::on_submit);
    nx::core::connect(clear_btn,  &button::clicked,
                      &ctrl, &form_controller::on_clear);
    nx::core::connect(name_edit,  &line_edit::return_pressed,
                      &ctrl, &form_controller::on_name_return);
    nx::core::connect(email_edit, &line_edit::return_pressed,
                      &ctrl, &form_controller::on_email_return);

    return app.exec();
}

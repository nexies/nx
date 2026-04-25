// widget_demo — Phase 3 smoke test.
//
// Tests the full widget pipeline:
//   screen (diff renderer) → v_box_layout → label + event_log
//   input_reader → input_dispatcher → screen::dispatch_* → focused widget
//
// Layout (manual, not v_box, because we want a fixed 1-row title):
//
//   row 0       ┌─ label ─────────────────────────┐  h = 1
//   row 1+      └─ event_log ─────────────────────┘  h = rows - 1
//
//  event_log internal rows:
//   0   live mouse position
//   1   separator
//   2+  scrolling key/mouse event log
//
// Press 'q' to quit.

#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include <fmt/format.h>

#include <nx/core2/thread/thread.hpp>
#include <nx/asio/io_context.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/input/input_reader.hpp>
#include <nx/tui/layouts/layout.hpp>
#include <nx/tui/types/size_policy.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/terminal/terminal.hpp>

using namespace nx::tui;

// ── label ─────────────────────────────────────────────────────────────────────
//
// Single-line widget that renders a text string in an optional color.

class label : public widget {
    std::string text_;
    color       fg_ = color::default_color;

public:
    NX_OBJECT(label)

    explicit label(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_vertical_policy(size_policy::fixed);
    }

    size_type size_hint() const override { return { 1, size().width }; }

    void set_text(std::string t) { text_ = std::move(t); update(); }
    void set_fg(color c)         { fg_   = c;            update(); }

protected:
    void on_paint(painter & p) override {
        p.set_color(fg_);
        p.draw_text({0, 0}, text_);
    }
};

// ── event_log ─────────────────────────────────────────────────────────────────
//
// Widget that logs key and mouse events.
// Receives events via the screen's event dispatch (must be focused for keys).

class event_log : public widget {
    static constexpr int k_mouse_row = 0;
    static constexpr int k_window_row = 1;
    static constexpr int k_sep_row   = 2;
    static constexpr int k_log_start = 3;

    std::string              mouse_line_ = "  mouse  -";
    std::string              window_line_ = "  window  -";
    std::vector<std::string> lines_;

public:
    NX_OBJECT(event_log)

    explicit event_log(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::strong_focus);
        auto app = tui_application::instance();
        if (app) {
            nx::core::connect(app, &tui_application::window_resized,
                this, &event_log::on_window_resized);
            format_window_size(terminal::get_window_size());
        } else {
            window_line_ = "tui_application not initialized... :C";
        }
    }

    void append(std::string line) {
        lines_.push_back(std::move(line));
        update();
    }

protected:
    void on_paint(painter & p) override {
        const int w     = size().width;
        const int h     = size().height;
        const int log_h = h - k_log_start;

        // Row 0: live mouse position.
        p.set_color(color::gray_dark);
        p.draw_text({0, k_mouse_row}, mouse_line_);
        p.draw_text({0, k_window_row}, window_line_);
        p.set_color(color::default_color);

        // Row 1: separator.
        p.set_color(color::gray_dark);
        p.draw_text({0, k_sep_row}, std::string(w, '-'));
        p.set_color(color::default_color);

        // Rows 2+: scrolling log — show the last log_h lines.
        if (log_h <= 0) return;
        const int start = std::max(0, static_cast<int>(lines_.size()) - log_h);
        for (int i = 0; i < log_h; ++i) {
            const int idx = start + i;
            if (idx < static_cast<int>(lines_.size())) {
                p.draw_text({0, k_log_start + i}, lines_[idx]);
            }
        }
    }

    // ── Event handlers ────────────────────────────────────────────────────────

    void on_key_press(key_event & e) override {
        if (e.code == key::printable) {
            const auto ch = e.character;
            if (ch >= 0x20 && ch <= 0x7E) {
                append(fmt::format("  KEY  '{}' (U+{:04X})  mods=[{}]",
                                   char(ch), uint32_t(ch), _mod_str(e.modifiers)));
            } else {
                append(fmt::format("  KEY  ctrl U+{:04X}  mods=[{}]",
                                   uint32_t(ch), _mod_str(e.modifiers)));
            }
        } else {
            append(fmt::format("  KEY  {}  mods=[{}]",
                               _key_name(e.code), _mod_str(e.modifiers)));
        }
    }

    void on_mouse_press(mouse_event & e) override {
        append(fmt::format("  MOUSE {} press  col={} row={}",
                           _btn_name(e.button), e.position.x, e.position.y));
    }

    void on_mouse_release(mouse_event & e) override {
        append(fmt::format("  MOUSE {} release  col={} row={}",
                           _btn_name(e.button), e.position.x, e.position.y));
    }

    void on_mouse_move(mouse_event & e) override {
        mouse_line_ = fmt::format("  mouse  col={:<4} row={:<4}  mods=[{}]",
                                  e.position.x, e.position.y,
                                  _mod_str(e.modifiers));
        update();
    }

    void on_wheel(mouse_event & e) override {
        const char * dir = (e.button == mouse_button::wheel_up) ? "up" : "down";
        append(fmt::format("  WHEEL {}  col={} row={}", dir,
                           e.position.x, e.position.y));
    }

    void format_window_size(window_size ws) {
        window_line_ = fmt::format(
            "  window pix_h={:<4} pix_w={:<4} char_h={:<4} char_w={:<4}",
            ws.pixels.height, ws.pixels.width,
            ws.chars.height, ws.chars.width);
    }

    void on_window_resized(window_size ws) {
        format_window_size(ws);
        update();
    }

private:
    static std::string _key_name(key k) {
        switch (k) {
        case key::enter:       return "Enter";
        case key::tab:         return "Tab";
        case key::escape:      return "Escape";
        case key::backspace:   return "Backspace";
        case key::delete_key:  return "Delete";
        case key::insert:      return "Insert";
        case key::home:        return "Home";
        case key::end:         return "End";
        case key::page_up:     return "PageUp";
        case key::page_down:   return "PageDown";
        case key::arrow_up:    return "Up";
        case key::arrow_down:  return "Down";
        case key::arrow_left:  return "Left";
        case key::arrow_right: return "Right";
        default:
            if (k >= key::f1 && k <= key::f12)
                return fmt::format("F{}", static_cast<int>(k)
                                         - static_cast<int>(key::f1) + 1);
            return "?";
        }
    }

    static std::string _btn_name(mouse_button b) {
        switch (b) {
        case mouse_button::left:   return "left";
        case mouse_button::middle: return "middle";
        case mouse_button::right:  return "right";
        default:                   return "?";
        }
    }

    static std::string _mod_str(key_modifiers m) {
        if (!m) return "";
        std::string s;
        if (m.has(key_modifier::ctrl))  s += "ctrl+";
        if (m.has(key_modifier::alt))   s += "alt+";
        if (m.has(key_modifier::shift)) s += "shift+";
        if (m.has(key_modifier::meta))  s += "meta+";
        if (!s.empty()) s.pop_back();
        return s;
    }
};


// ── input_dispatcher ──────────────────────────────────────────────────────────
//
// Bridges input_reader signals → screen event dispatch + render.

class input_dispatcher : public nx::core::object {
    tui_application * app_;

public:
    NX_OBJECT(input_dispatcher)

    explicit input_dispatcher(tui_application * app,
                              nx::core::object * parent = nullptr)
        : nx::core::object(parent), app_(app)
    {}

    void on_key(key_event e) {
        if (e.code == key::printable && e.character == 'q') {
            app_->quit();
            return;
        }
        if (auto * s = app_->main_screen()) {
            s->dispatch_key_press(e);
            s->render();
        }
    }

    void on_mouse(mouse_event e) {
        if (auto * s = app_->main_screen()) {
            s->dispatch_mouse(e);
            s->render();
        }
    }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto *          scr = app.main_screen();

    // ── Widget tree ───────────────────────────────────────────────────────────
    //
    // screen  (v_box_layout)
    //   ├── label      fixed height 1
    //   └── event_log  expanding — fills the rest

    auto * title = new label(scr);
    title->set_text("  nx::tui  Widget Demo  —  press 'q' to quit");
    title->set_fg(color::cyan_bright);
    // label already has vertical_policy = fixed; size_hint().height = 1

    auto * log = new event_log(scr);
    // event_log uses default expanding vertical policy

    scr->set_layout(std::make_unique<v_box_layout>());
    scr->set_focused_widget(log);

    // ── Input routing ─────────────────────────────────────────────────────────

    input_reader     reader;
    input_dispatcher dispatcher(&app);

    nx::core::connect(&reader, &input_reader::key_pressed,
                      &dispatcher, &input_dispatcher::on_key);
    nx::core::connect(&reader, &input_reader::mouse_input,
                      &dispatcher, &input_dispatcher::on_mouse);
    reader.start();

    return app.exec();
}

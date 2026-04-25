// input_demo — Phase 2 smoke test.
//
// Enters alt-screen + raw mode + mouse tracking, then prints every
// key/mouse event to the screen.  Press 'q' to quit.

#include <cstdio>
#include <string>

#include <fmt/format.h>
#include <nx/core2/thread/thread.hpp>
#include <nx/asio/io_context.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/input/input_reader.hpp>
#include <nx/tui/terminal/terminal.hpp>

using namespace nx::tui;

// ── Formatting helpers ────────────────────────────────────────────────────────

namespace {

std::string key_name(key k)
{
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
    case key::f1:          return "F1";
    case key::f2:          return "F2";
    case key::f3:          return "F3";
    case key::f4:          return "F4";
    case key::f5:          return "F5";
    case key::f6:          return "F6";
    case key::f7:          return "F7";
    case key::f8:          return "F8";
    case key::f9:          return "F9";
    case key::f10:         return "F10";
    case key::f11:         return "F11";
    case key::f12:         return "F12";
    default:               return "?";
    }
}

std::string mod_str(key_modifiers mods)
{
    if (!mods) return "";
    std::string s;
    if (mods.has(key_modifier::ctrl))  s += "ctrl+";
    if (mods.has(key_modifier::alt))   s += "alt+";
    if (mods.has(key_modifier::shift)) s += "shift+";
    if (mods.has(key_modifier::meta))  s += "meta+";
    if (!s.empty()) s.pop_back(); // remove trailing '+'
    return s;
}

void flush_output()
{
    fflush(terminal::get_output_stream());
}

// ── Demo handler ──────────────────────────────────────────────────────────────
//
// nx::core::connect() requires an nx::core::object receiver, so we put
// the event-display logic in a small object.

class demo_handler : public nx::core::object
{
    // Row layout:
    //   1   title bar
    //   2   separator
    //   3   live mouse position (updated in-place)
    //   4   separator
    //   5+  scrolling event log
    static constexpr int k_mouse_row = 3;
    static constexpr int k_first_row = 5;
    static constexpr int k_max_row   = 42;

public:
    NX_OBJECT(demo_handler)

    explicit demo_handler(tui_application * app,
                          nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , app_(app)
    {}

    void on_key(key_event e)
    {
        if (e.code == key::printable && e.character == 'q') {
            app_->quit();
            return;
        }

        std::string line;
        if (e.code == key::printable) {
            const auto ch = e.character;
            if (ch >= 0x20 && ch <= 0x7E)
                line = fmt::format("  KEY  '{}' (U+{:04X})  mods=[{}]",
                                   char(ch), uint32_t(ch), mod_str(e.modifiers));
            else
                line = fmt::format("  KEY  ctrl-code U+{:04X}  mods=[{}]",
                                   uint32_t(ch), mod_str(e.modifiers));
        } else {
            line = fmt::format("  KEY  {}  mods=[{}]",
                               key_name(e.code), mod_str(e.modifiers));
        }
        _append_line(line);
    }

    void on_mouse(mouse_event e)
    {
        if (e.action == mouse_action::move) {
            // Update the live position line without touching the event log.
            terminal::move_cursor(k_mouse_row, 1);
            terminal::erase_to_line_end();
            terminal::set_color(color::gray_dark);
            terminal::print("  mouse ");
            terminal::set_color(color::default_color);
            terminal::print(fmt::format("col={:<4} row={:<4}  mods=[{}]",
                                        e.position.x, e.position.y,
                                        mod_str(e.modifiers)));
            flush_output();
            return;
        }

        const char * btn = "?";
        switch (e.button) {
        case mouse_button::left:        btn = "left";        break;
        case mouse_button::middle:      btn = "middle";      break;
        case mouse_button::right:       btn = "right";       break;
        case mouse_button::wheel_up:    btn = "wheel-up";    break;
        case mouse_button::wheel_down:  btn = "wheel-down";  break;
        case mouse_button::none:        btn = "none";        break;
        }

        const char * act = (e.action == mouse_action::press) ? "press" : "release";

        _append_line(fmt::format("  MOUSE {} {} col={} row={}  mods=[{}]",
                                 btn, act,
                                 e.position.x, e.position.y,
                                 mod_str(e.modifiers)));
    }

private:
    tui_application * app_;
    int row_ = k_first_row;

    void _append_line(const std::string & text)
    {
        terminal::move_cursor(row_, 1);
        terminal::erase_to_line_end();
        terminal::print(text);
        flush_output();
        if (++row_ > k_max_row)
            row_ = k_first_row;
    }
};

void draw_header()
{
    // Row 1: title
    terminal::move_cursor(1, 1);
    terminal::erase_to_line_end();
    terminal::set_color(color::cyan_bright);
    terminal::print("  nx::tui  Input Demo");
    terminal::set_color(color::default_color);
    terminal::print("  —  press 'q' to quit");

    // Row 2: separator
    terminal::move_cursor(2, 1);
    terminal::erase_to_line_end();
    terminal::set_color(color::gray_dark);
    terminal::print("  ──────────────────────────────────────────────────────");
    terminal::set_color(color::default_color);

    // Row 3: live mouse position (placeholder until first mouse event)
    terminal::move_cursor(3, 1);
    terminal::erase_to_line_end();
    terminal::set_color(color::gray_dark);
    terminal::print("  mouse  —");
    terminal::set_color(color::default_color);

    // Row 4: separator before event log
    terminal::move_cursor(4, 1);
    terminal::erase_to_line_end();
    terminal::set_color(color::gray_dark);
    terminal::print("  ──────────────────────────────────────────────────────");
    terminal::set_color(color::default_color);

    flush_output();
}

} // anonymous namespace

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);

    input_reader  reader;
    demo_handler  handler(&app);

    nx::core::connect(&reader, &input_reader::key_pressed,
                      &handler, &demo_handler::on_key);
    nx::core::connect(&reader, &input_reader::mouse_input,
                      &handler, &demo_handler::on_mouse);

    reader.start();

    // draw_header() must run after exec() sets up the alt buffer + raw mode.
    // Posting it ensures it executes at the first event-loop iteration.
    nx::asio::post(nx::core::thread::current_context(), [] { draw_header(); });

    return app.exec();
}

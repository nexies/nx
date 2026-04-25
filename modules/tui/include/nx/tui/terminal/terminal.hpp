#pragma once

#include <nx/common/helpers.hpp>
#include <fmt/format.h>

#include <nx/tui/types/size.hpp>
#include <nx/tui/types/color.hpp>
#include <nx/tui/graphics/pixel.hpp>
#include <nx/tui/types/point.hpp>

#include <cstdio>

namespace nx::tui {

// ──────────────────────────────────────────────────────────────────────────────
// terminal — static utility for low-level ANSI terminal control.
// All methods write to the configured output stream (default: stderr).
// ──────────────────────────────────────────────────────────────────────────────

class terminal
{
    inline static FILE * ostream_ = stderr;

public:
    enum class mode {
        text_mono_40x25,
        text_color_40x25,
        text_mono_80x25,
        text_color_80x25,
        graph_4color_320x200,
        graph_mono_320x200,
        graph_mono_640x200,
        graph_color_320x200,
        graph_color_640x200,
        graph_mono_640x350,
        graph_16color_640x350,
        graph_mono_640x480,
        graph_16color_640x480,
        graph_256color_320x200,
    };

    // ── Output ────────────────────────────────────────────────────────────────

    template<typename... Args>
    static void print(fmt::format_string<Args...> fmt_str, Args &&... args)
    {
        fmt::print(ostream_, fmt_str, std::forward<Args>(args)...);
    }

    static void print(std::string_view text)  { fmt::print(ostream_, "{}", text); }
    static void print(char ch)                { fmt::print(ostream_, "{}", ch);   }

    static void        set_output(FILE * stream);
    [[nodiscard]] static FILE * get_output_stream();

    // ── Capabilities ──────────────────────────────────────────────────────────

    [[nodiscard]] static window_size  get_window_size();
    [[nodiscard]] static color_type   get_color_support();
    static void                       set_color_support(color_type type);

    // ── Cursor movement ───────────────────────────────────────────────────────

    static void move_cursor_home();
    static void move_cursor(int row, int col);
    static void move_cursor_up(int rows);
    static void move_cursor_down(int rows);
    static void move_cursor_right(int cols);
    static void move_cursor_left(int cols);
    static void move_cursor_next_line_begin(int lines);
    static void move_cursor_prev_line_begin(int lines);
    static void move_cursor_to_column(int col);

    [[nodiscard]] static point<int> get_cursor_pos();

    static void save_cursor();
    static void restore_cursor();
    static void set_cursor_visible(bool visible);

    // ── Scroll ────────────────────────────────────────────────────────────────

    static void scroll_up();

    // ── Erase ─────────────────────────────────────────────────────────────────

    static void erase_in_display();
    static void erase_to_screen_end();
    static void erase_to_screen_begin();
    static void erase_screen();
    static void erase_saved_lines();
    static void erase_in_line();
    static void erase_to_line_end();
    static void erase_to_line_begin();
    static void erase_line();

    // ── Text style ────────────────────────────────────────────────────────────

    static void enable_dim(bool enable);
    static void enable_italic(bool enable);
    static void enable_underline(bool enable);
    static void enable_blinking(bool enable);
    static void enable_inverse(bool enable);
    static void enable_hidden(bool enable);
    static void enable_line_wrap(bool enable);
    static void enable_strike_through(bool enable);

    static void set_pixel_style(pixel_style style);
    static void reset_pixel_style(pixel_style style);

    // ── Color ─────────────────────────────────────────────────────────────────

    static void set_color(color c);
    static void set_background_color(color c);

    // ── Screen / buffer ───────────────────────────────────────────────────────

    static void save_screen();
    static void restore_screen();
    static void enable_alt_buffer();
    static void disable_alt_buffer();
    static void set_screen_mode(mode m);
    static void reset_screen_mode(mode m);

    // ── Input mode ────────────────────────────────────────────────────────────

    static void enable_raw_mode();
    static void disable_raw_mode();
    static void enable_mouse_tracking();
    static void disable_mouse_tracking();
};

} // namespace nx::tui

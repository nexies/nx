#include <nx/common/platform.hpp>

#if defined(NX_POSIX)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#elif defined(NX_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#error "Unsupported platform. Please reconfigure"
#endif

#include <fmt/format.h>

#include <nx/tui/terminal/terminal.hpp>

#include "ansi_codes.p.hpp"

using namespace nx::tui;

// ── Capabilities ──────────────────────────────────────────────────────────────

namespace
{
    window_size fallback_size ()
    {
        return window_size{{10, 10}, {100, 100}};
    }
}


window_size terminal::get_window_size()
{
#if defined(NX_OS_WINDOWS)
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return window_size{csbi.srWindow.Bottom - csbi.srWindow.Top + 1,
            csbi.srWindow.Right - csbi.srWindow.Left + 1
        };
    }

    return fallback_size();
#else

    winsize w { 0 };
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return { { w.ws_row, w.ws_col }, { w.ws_ypixel, w.ws_xpixel } };

#endif
}

namespace {
    bool       g_color_support_cached = false;
    color_type g_color_support;

    bool str_contains(const std::string & s, const char * key) {
        return s.find(key) != std::string::npos;
    }

    color_type detect_color_support()
    {
#if defined(__EMSCRIPTEN__)
        return color_type::true_color;
#elif defined (NX_OS_WINDOWS)
        return color_type::true_color;
#endif
        const char * p;
        std::string COLORTERM = (p = std::getenv("COLORTERM")) ? p : "";  // NOLINT
        if (str_contains(COLORTERM, "24bit") || str_contains(COLORTERM, "truecolor"))
            return color_type::true_color;

        std::string TERM = (p = std::getenv("TERM")) ? p : "";  // NOLINT
        if (str_contains(COLORTERM, "256") || str_contains(TERM, "256"))
            return color_type::palette256;

        return color_type::palette16;
    }
} // anonymous namespace

color_type terminal::get_color_support()
{
    if (!g_color_support_cached) {
        g_color_support = detect_color_support();
        g_color_support_cached = true;
    }
    return g_color_support;
}

void terminal::set_color_support(color_type type)
{
    g_color_support        = type;
    g_color_support_cached = true;
}

void terminal::set_output(FILE * stream) { ostream_ = stream; }
FILE * terminal::get_output_stream()     { return ostream_; }

// ── Frame buffering ───────────────────────────────────────────────────────────

void terminal::begin_frame()
{
    s_frame_buf_.clear();
    s_frame_active_ = true;
}

void terminal::end_frame()
{
    s_frame_active_ = false;
    if (!s_frame_buf_.empty()) {
        std::fwrite(s_frame_buf_.data(), 1, s_frame_buf_.size(), ostream_);
        std::fflush(ostream_);
    }
}

// ── Cursor movement ───────────────────────────────────────────────────────────

void terminal::move_cursor_home()                { print(ansi::g_cursor_home_cmd); }
void terminal::move_cursor(int row, int col)     { print(ansi::g_cursor_pos_cmd, row, col); }
void terminal::move_cursor_up(int rows)          { print(ansi::g_cursor_up_cmd, rows); }
void terminal::move_cursor_down(int rows)        { print(ansi::g_cursor_down_cmd, rows); }
void terminal::move_cursor_right(int cols)       { print(ansi::g_cursor_right_cmd, cols); }
void terminal::move_cursor_left(int cols)        { print(ansi::g_cursor_left_cmd, cols); }
void terminal::move_cursor_next_line_begin(int n){ print(ansi::g_cursor_next_begin_cmd, n); }
void terminal::move_cursor_prev_line_begin(int n){ print(ansi::g_cursor_prev_begin_cmd, n); }
void terminal::move_cursor_to_column(int col)   { print(ansi::g_cursor_to_column_cmd, col); }

point<int> terminal::get_cursor_pos()
{
    fmt::print(ostream_, ansi::g_request_cursor_pos_cmd);
    int x = 0, y = 0;
    std::fscanf(ostream_, CSI "%i;%iR", &x, &y);
    return { x, y };
}

void terminal::scroll_up()     { print(ansi::g_scroll_up_cmd); }
void terminal::save_cursor()   { print(ansi::g_save_cursor_cmd); }
void terminal::restore_cursor(){ print(ansi::g_restore_cursor_cmd); }

// ── Erase ─────────────────────────────────────────────────────────────────────

void terminal::erase_in_display()    { print(ansi::g_erase_in_display_cmd); }
void terminal::erase_to_screen_end() { print(ansi::g_erase_to_screen_end_cmd); }
void terminal::erase_to_screen_begin(){ print(ansi::g_erase_to_screen_begin_cmd); }
void terminal::erase_screen()        { print(ansi::g_erase_screen_cmd); }
void terminal::erase_saved_lines()   { print(ansi::g_erase_saved_lines_cmd); }
void terminal::erase_in_line()       { print(ansi::g_erase_in_line_cmd); }
void terminal::erase_to_line_end()   { print(ansi::g_erase_to_line_end_cmd); }
void terminal::erase_to_line_begin() { print(ansi::g_erase_to_line_start_cmd); }
void terminal::erase_line()          { print(ansi::g_erase_line_cmd); }

// ── Text style ────────────────────────────────────────────────────────────────

void terminal::enable_bold(bool e)         { print(e ? ansi::g_set_bold_cmd         : ansi::g_reset_bold_cmd); }
void terminal::enable_dim(bool e)          { print(e ? ansi::g_set_dim_cmd          : ansi::g_reset_dim_cmd); }
void terminal::enable_italic(bool e)       { print(e ? ansi::g_set_italic_cmd       : ansi::g_reset_italic_cmd); }
void terminal::enable_underline(bool e)    { print(e ? ansi::g_set_underline_cmd    : ansi::g_reset_underline_cmd); }
void terminal::enable_blinking(bool e)     { print(e ? ansi::g_set_blinking_cmd     : ansi::g_reset_blinking_cmd); }
void terminal::enable_inverse(bool e)      { print(e ? ansi::g_set_inverse_cmd      : ansi::g_reset_inverse_cmd); }
void terminal::enable_hidden(bool e)       { print(e ? ansi::g_set_hidden_cmd       : ansi::g_reset_hidden_cmd); }
void terminal::enable_line_wrap(bool e)    { print(e ? ansi::g_set_screen_mode_line_wrap_cmd : ansi::g_reset_screen_mode_line_wrap_cmd); }
void terminal::enable_strike_through(bool e){ print(e ? ansi::g_set_strike_through_cmd : ansi::g_reset_strike_through_cmd); }

void terminal::set_pixel_style(pixel_style style)
{
    if (!style) return;
    if (style.has(pixel_style_flag::bold))           enable_bold(true);
    if (style.has(pixel_style_flag::blink))          enable_blinking(true);
    if (style.has(pixel_style_flag::dim))            enable_dim(true);
    if (style.has(pixel_style_flag::inverted))       enable_inverse(true);
    if (style.has(pixel_style_flag::italic))         enable_italic(true);
    if (style.has(pixel_style_flag::strike_through)) enable_strike_through(true);
    if (style.has(pixel_style_flag::underline))      enable_underline(true);
    if (style.has(pixel_style_flag::underline_double)) enable_underline(true);
}

void terminal::reset_pixel_style(pixel_style style)
{
    if (!style) return;
    if (style.has(pixel_style_flag::bold))           enable_bold(false);
    if (style.has(pixel_style_flag::blink))          enable_blinking(false);
    if (style.has(pixel_style_flag::dim))            enable_dim(false);
    if (style.has(pixel_style_flag::inverted))       enable_inverse(false);
    if (style.has(pixel_style_flag::italic))         enable_italic(false);
    if (style.has(pixel_style_flag::strike_through)) enable_strike_through(false);
    if (style.has(pixel_style_flag::underline))      enable_underline(false);
    if (style.has(pixel_style_flag::underline_double)) enable_underline(false);
}

// ── Color ─────────────────────────────────────────────────────────────────────

void terminal::set_color(color c)            { print(c.to_ansi()); }
void terminal::set_background_color(color c) { print(c.to_ansi(true)); }

// ── Cursor visibility ─────────────────────────────────────────────────────────

void terminal::set_cursor_visible(bool visible)
{
    print(visible ? ansi::g_set_cursor_visible_cmd : ansi::g_set_cursor_invisible_cmd);
}

// ── Screen / buffer ───────────────────────────────────────────────────────────

void terminal::save_screen()      { print(ansi::g_save_screen_cmd); }
void terminal::restore_screen()   { print(ansi::g_restore_screen_cmd); }
void terminal::enable_alt_buffer() { print(ansi::g_enable_alt_buffer_cmd); }
void terminal::disable_alt_buffer(){ print(ansi::g_disable_alt_buffer_cmd); }

void terminal::set_screen_mode(mode /*m*/)   { /* TODO */ }
void terminal::reset_screen_mode(mode /*m*/) { /* TODO */ }

// ── Raw mode ──────────────────────────────────────────────────────────────────

namespace {
    bool g_raw_mode_active = false;
#if defined(NX_POSIX)
    termios g_saved_termios {};
#elif defined(NX_OS_WINDOWS)
    DWORD g_saved_input_mode   = 0;
    DWORD g_saved_stdout_mode  = 0;
    DWORD g_saved_stderr_mode  = 0;
    UINT  g_saved_output_cp    = 0;
#endif
} // anonymous namespace

void terminal::enable_raw_mode()
{
    if (g_raw_mode_active)
        return;

#if defined(NX_POSIX)
    if (tcgetattr(STDIN_FILENO, &g_saved_termios) != 0)
        return;

    termios raw = g_saved_termios;
    cfmakeraw(&raw);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

#elif defined(NX_OS_WINDOWS)
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn == INVALID_HANDLE_VALUE)
        return;
    if (!GetConsoleMode(hIn, &g_saved_input_mode))
        return;
    // Use native Win32 input events — no VT translation needed.
    // ENABLE_QUICK_EDIT_MODE must be cleared: if left active it intercepts left
    // mouse button clicks for text selection and suppresses MOUSE_EVENT records.
    // ENABLE_EXTENDED_FLAGS is required for ENABLE_MOUSE_INPUT to report scroll.
    DWORD raw_in = (g_saved_input_mode
        & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT
            | ENABLE_QUICK_EDIT_MODE))
        | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hIn, raw_in);

    // Enable VT escape-sequence processing on both stdout and stderr so ANSI
    // codes are interpreted on whichever handle ostream_ points to.
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &g_saved_stdout_mode))
        SetConsoleMode(hOut, g_saved_stdout_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    if (hErr != INVALID_HANDLE_VALUE && GetConsoleMode(hErr, &g_saved_stderr_mode))
        SetConsoleMode(hErr, g_saved_stderr_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // Switch the console output codepage to UTF-8 so that multi-byte sequences
    // (box-drawing, CJK, emoji) are rendered as single glyphs rather than as
    // individual bytes interpreted by the legacy codepage.
    g_saved_output_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif

    g_raw_mode_active = true;
}

void terminal::disable_raw_mode()
{
    if (!g_raw_mode_active)
        return;

#if defined(NX_POSIX)
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_saved_termios);
#elif defined(NX_OS_WINDOWS)
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE)
        SetConsoleMode(hIn, g_saved_input_mode);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE)
        SetConsoleMode(hOut, g_saved_stdout_mode);
    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    if (hErr != INVALID_HANDLE_VALUE)
        SetConsoleMode(hErr, g_saved_stderr_mode);
    if (g_saved_output_cp != 0)
        SetConsoleOutputCP(g_saved_output_cp);
#endif

    g_raw_mode_active = false;
}

// ── Mouse tracking ────────────────────────────────────────────────────────────
//
// Uses the SGR extended mouse protocol (?1006) on top of any-event tracking
// (?1003). This combination reports all button events + motion with accurate
// coordinates and distinguishes press vs. release.

void terminal::enable_mouse_tracking()
{
#if defined(NX_OS_WINDOWS)
    // On Windows, mouse input is controlled by ENABLE_MOUSE_INPUT in the
    // console mode, which is already set in enable_raw_mode().  ANSI tracking
    // sequences are not needed.
#else
    fmt::print(ostream_, ansi::g_enable_mouse_any_cmd);   // report all motion
    fmt::print(ostream_, ansi::g_enable_mouse_sgr_cmd);   // SGR encoding
    fflush(ostream_);
#endif
}

void terminal::disable_mouse_tracking()
{
#if defined(NX_OS_WINDOWS)
    // Nothing to do — raw mode is disabled in disable_raw_mode().
#else
    fmt::print(ostream_, ansi::g_disable_mouse_sgr_cmd);
    fmt::print(ostream_, ansi::g_disable_mouse_any_cmd);
    fflush(ostream_);
#endif
}

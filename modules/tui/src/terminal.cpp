//
// Created by nexie on 03.03.2026.
//

#include <sys/ioctl.h>
#include <unistd.h>

#include <fmt/format.h>

#include <nx/tui/terminal/terminal.hpp>

#include "ansi_codes.p.hpp"

using namespace nx::tui;

// #ifdef WIN32
// #define NX_TUI_MICROSOFT_TERMINAL_FALLBACK
// #endif


WindowSize Terminal::GetWindowSize()
{
    winsize w { 0 };
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_row, w.ws_col, w.ws_xpixel, w.ws_ypixel};
}

const char* Safe(const char* c) {
    return (c != nullptr) ? c : "";
}

bool Contains(const std::string& s, const char* key) {
    return s.find(key) != std::string::npos;
}

ColorType g_color_support;
bool g_color_support_cached = false;

ColorType ComputeColorSupport()
{
#if defined(__EMSCRIPTEN__)
    return ColorType::TrueColor;
#endif

    std::string COLORTERM = Safe(std::getenv("COLORTERM"));  // NOLINT
    if (Contains(COLORTERM, "24bit") || Contains(COLORTERM, "truecolor")) {
        return ColorType::TrueColor;
    }

    std::string TERM = Safe(std::getenv("TERM"));  // NOLINT
    if (Contains(COLORTERM, "256") || Contains(TERM, "256")) {
        return ColorType::Palette256;
    }

#if defined(NX_TUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft terminals do not properly declare themselves supporting true
    // colors: https://github.com/microsoft/terminal/issues/1040
    // As a fallback, assume microsoft terminal are the ones not setting those
    // variables, and enable true colors.
    if (TERM.empty() && COLORTERM.empty()) {
        return ColorType::TrueColor;
    }
#endif

    return ColorType::Palette16;
}

ColorType Terminal::GetColorSupport()
{
    if (g_color_support_cached)
        return g_color_support;

    return g_color_support = ComputeColorSupport();
}

void Terminal::SetColorSupport(ColorType type)
{
    g_color_support = type;
    g_color_support_cached = true;
}

void Terminal::SetOutput(FILE* stream)
{
    ostream_ = stream;
}

FILE* Terminal::GetOutputStream()
{
    return ostream_;
}

void Terminal::MoveCursorHome()
{
    fmt::print(ostream_, ansi::g_cursor_home_cmd);
}

void Terminal::MoveCursor(int line, int column)
{
    fmt::print(ostream_, ansi::g_cursor_pos_cmd, line, column);
}

void Terminal::MoveCursorUp(int rows)
{
    fmt::print(ostream_, ansi::g_cursor_up_cmd, rows);
}

void Terminal::MoveCursorDown(int rows)
{
    fmt::print(ostream_, ansi::g_cursor_down_cmd, rows);
}

void Terminal::MoveCursorRight(int columns)
{
    fmt::print(ostream_, ansi::g_cursor_right_cmd, columns);
}

void Terminal::MoveCursorLeft(int columns)
{
    fmt::print(ostream_, ansi::g_cursor_left_cmd, columns);
}

void Terminal::MoveCursorNextLineBegin(int lines)
{
    fmt::print(ostream_, ansi::g_cursor_next_begin_cmd, lines);
}

void Terminal::MoveCursorPrevLineBegin(int lines)
{
    fmt::print(ostream_, ansi::g_cursor_prev_begin_cmd, lines);
}

void Terminal::MoveCursorToColumn(int column)
{
    fmt::print(ostream_, ansi::g_cursor_to_column_cmd, column);
}

Point<int> Terminal::GetCursorPos()
{
    fmt::print(ostream_, ansi::g_request_cursor_pos_cmd);
    int x, y;
    std::fscanf(ostream_, CSI "%i;%iR", &x, &y);
    return {x, y};
}

void Terminal::ScrollUp()
{
    fmt::print(ostream_, ansi::g_scroll_up_cmd);
}

void Terminal::SaveCursor()
{
    fmt::print(ostream_, ansi::g_save_cursor_cmd);
}

void Terminal::RestoreCursor()
{
    fmt::print(ostream_, ansi::g_restore_cursor_cmd);
}

void Terminal::EraseInDisplay()
{
    fmt::print(ostream_, ansi::g_erase_in_display_cmd);
}

void Terminal::EraseToScreenEnd()
{
    fmt::print(ostream_, ansi::g_erase_to_screen_end_cmd);
}

void Terminal::EraseToScreenBegin()
{
    fmt::print(ostream_, ansi::g_erase_to_screen_begin_cmd);
}

void Terminal::EraseScreen()
{
    fmt::print(ostream_, ansi::g_erase_screen_cmd);
}

void Terminal::EraseSavedLines()
{
    fmt::print(ostream_, ansi::g_erase_saved_lines_cmd);
}

void Terminal::EraseInLine()
{
    fmt::print(ostream_, ansi::g_erase_in_line_cmd);
}

void Terminal::EraseToLineEnd()
{
    fmt::print(ostream_, ansi::g_erase_to_line_end_cmd);
}

void Terminal::EraseToLineBegin()
{
    fmt::print(ostream_, ansi::g_erase_to_line_start_cmd);
}

void Terminal::EraseLine()
{
    fmt::print(ostream_, ansi::g_erase_line_cmd);
}

void Terminal::EnableDim(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_dim_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_dim_cmd);
}

void Terminal::EnableItalic(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_italic_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_italic_cmd);
}

void Terminal::EnableUnderline(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_underline_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_underline_cmd);
}

void Terminal::EnableBlinking(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_blinking_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_blinking_cmd);
}

void Terminal::EnableInverse(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_inverse_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_inverse_cmd);
}

void Terminal::EnableHidden(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_hidden_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_hidden_cmd);
}

void Terminal::EnableLineWrap(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_screen_mode_line_wrap_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_screen_mode_line_wrap_cmd);
}

void Terminal::EnableStrikeThrough(bool enable)
{
    if (enable)
        fmt::print(ostream_, ansi::g_set_strike_through_cmd);
    else
        fmt::print(ostream_, ansi::g_reset_strike_through_cmd);
}

void Terminal::SetPixelStyle(PixelStyle style)
{
    if (!style)
        return;

    if (style.has(PixelStyleOptions::Blink))
        EnableBlinking(true);
    if (style.has(PixelStyleOptions::Dim))
        EnableDim(true);
    if (style.has(PixelStyleOptions::Inverted))
        EnableInverse(true);
    if (style.has(PixelStyleOptions::Italic))
        EnableItalic(true);
    if (style.has(PixelStyleOptions::StrikeThrough))
        EnableStrikeThrough(true);
    if (style.has(PixelStyleOptions::Underlined))
        EnableUnderline(true);
    if (style.has(PixelStyleOptions::UnderlinedDouble))
        EnableUnderline(true);
}

void Terminal::ResetPixelStyle(PixelStyle style)
{
    if (!style)
        return;

    if (style.has(PixelStyleOptions::Blink))
        EnableBlinking(false);
    if (style.has(PixelStyleOptions::Dim))
        EnableDim(false);
    if (style.has(PixelStyleOptions::Inverted))
        EnableInverse(false);
    if (style.has(PixelStyleOptions::Italic))
        EnableItalic(false);
    if (style.has(PixelStyleOptions::StrikeThrough))
        EnableStrikeThrough(false);
    if (style.has(PixelStyleOptions::Underlined))
        EnableUnderline(false);
    if (style.has(PixelStyleOptions::UnderlinedDouble))
        EnableUnderline(false);
}

void Terminal::SetColor(Color color)
{
    Print(color.print());
}

void Terminal::SetBackgroundColor(Color color)
{
    Print(color.print(true));
}

void Terminal::SetCursorVisible(bool visible)
{
    if (visible)
        fmt::print(ostream_, ansi::g_set_cursor_visible_cmd);
    else
        fmt::print(ostream_, ansi::g_set_cursor_invisible_cmd);
}

void Terminal::SaveScreen()
{
    fmt::print(ostream_, ansi::g_save_screen_cmd);
}

void Terminal::RestoreScreen()
{
    fmt::print(ostream_, ansi::g_restore_screen_cmd);
}

void Terminal::EnableAltBuffer()
{
    fmt::print(ostream_, ansi::g_enable_alt_buffer_cmd);
}

void Terminal::DisableAltBuffer()
{
    fmt::print(ostream_, ansi::g_disable_alt_buffer_cmd);
}

void Terminal::EnableRawMode()
{
}

void Terminal::DisableRawMode()
{
}

void Terminal::EnableMouseTracking()
{
}

void Terminal::DisableMouseTracking()
{
}



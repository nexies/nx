//
// Created by nexie on 03.03.2026.
//

#include <sys/ioctl.h>
#include <unistd.h>

#include <nx/tui/terminal/Terminal.hpp>

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


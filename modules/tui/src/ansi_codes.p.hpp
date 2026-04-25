//
// Created by nexie on 18.03.2026.
//

#ifndef ANSI_CODES_P_HPP
#define ANSI_CODES_P_HPP

#include <string>
#include <nx/macro/util/platform.hpp>
#include <nx/macro/util/overload.hpp>
#include <nx/macro/args/append.hpp>
#include <fmt/format.h>

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#escape

# define ESC "\x1B"
    // # define CSI "\x9B"
# define CSI ESC "["
# define DCS "\x90"
# define OSC "\x9D"

# define _ansi(esc, code) esc code
# define _ansi_1(esc, code) esc "{}" code
# define _ansi_2(esc, code) esc "{};{}" code

# define _csi(code) _ansi(CSI, code)
# define _csi_1(code) _ansi_1(CSI, code)
# define _csi_2(code) _ansi_2(CSI, code)


# define ANSI_CMD_1(name) g_##name##_cmd
# define ANSI_CMD_2(name, p) ANSI_CMD_1(name), p
# define ANSI_CMD_3(name, p1, p2) ANSI_CMD_1(name), p1, p2

# define ANSI_CMD(name, ...) \
    NX_OVERLOAD(ANSI_CMD, name _nx_append_args(__VA_ARGS__))

# define DECL_ANSI_CMD(name, value) \
    constexpr inline std::string_view ANSI_CMD(name) = value;

namespace nx::tui::ansi {
    DECL_ANSI_CMD(terminal_bell,         "\a")
    DECL_ANSI_CMD(backspace,             "\b")
    DECL_ANSI_CMD(horizontal_tab,        "\t")
    DECL_ANSI_CMD(new_line,              "\n")
    DECL_ANSI_CMD(vertical_tab,          "\v")
    DECL_ANSI_CMD(new_page,              "\f")
    DECL_ANSI_CMD(return,                "\r")

    DECL_ANSI_CMD(cursor_home,           _csi("H"))
    DECL_ANSI_CMD(cursor_pos,            _csi_2("H"))
    DECL_ANSI_CMD(cursor_up,             _csi_1("A"))
    DECL_ANSI_CMD(cursor_down,           _csi_1("B"))
    DECL_ANSI_CMD(cursor_right,          _csi_1("C"))
    DECL_ANSI_CMD(cursor_left,           _csi_1("D"))
    DECL_ANSI_CMD(cursor_next_begin,     _csi_1("E"))
    DECL_ANSI_CMD(cursor_prev_begin,     _csi_1("F"))
    DECL_ANSI_CMD(cursor_to_column,      _csi_1("G"))
    DECL_ANSI_CMD(request_cursor_pos,    _csi("6n");)
    DECL_ANSI_CMD(scroll_up,             _ansi(ESC, "M"))
    DECL_ANSI_CMD(save_cursor,           _ansi(ESC, "7"))
    DECL_ANSI_CMD(restore_cursor,        _ansi(ESC, "8"))

    DECL_ANSI_CMD(erase_in_display,      _csi("J"))
    DECL_ANSI_CMD(erase_to_screen_end,   _csi("0J"))
    DECL_ANSI_CMD(erase_to_screen_begin, _csi("1J"))
    DECL_ANSI_CMD(erase_screen,          _csi("2J"))
    DECL_ANSI_CMD(erase_saved_lines,     _csi("3J"))
    DECL_ANSI_CMD(erase_in_line,         _csi("K"))
    DECL_ANSI_CMD(erase_to_line_end,     _csi("0K"))
    DECL_ANSI_CMD(erase_to_line_start,   _csi("1K"))
    DECL_ANSI_CMD(erase_line,            _csi("2K"))

# define DECL_ANSI_CHAR_STYLE_CMD(name, code) \
    DECL_ANSI_CMD(set_##name, _csi(code "m")) \
    DECL_ANSI_CMD(reset_##name, _csi("2" code "m"))

    DECL_ANSI_CMD(reset_style,               _csi("0m") )
    DECL_ANSI_CMD(set_bold,                  _csi("1m") )
    DECL_ANSI_CMD(reset_bold,                _csi("22m") )

    DECL_ANSI_CHAR_STYLE_CMD(dim,            "2")
    DECL_ANSI_CHAR_STYLE_CMD(italic,         "3")
    DECL_ANSI_CHAR_STYLE_CMD(underline,      "4")
    DECL_ANSI_CHAR_STYLE_CMD(blinking,       "5")
    DECL_ANSI_CHAR_STYLE_CMD(inverse,        "7")
    DECL_ANSI_CHAR_STYLE_CMD(hidden,         "8")
    DECL_ANSI_CHAR_STYLE_CMD(strike_through, "9")

    DECL_ANSI_CMD(set_cursor_invisible, _csi("?25l"))
    DECL_ANSI_CMD(set_cursor_visible,   _csi("?25h"))
    DECL_ANSI_CMD(restore_screen,       _csi("?47l"))
    DECL_ANSI_CMD(save_screen,          _csi("?47h"))
    DECL_ANSI_CMD(disable_alt_buffer,   _csi("?1049l"))
    DECL_ANSI_CMD(enable_alt_buffer,    _csi("?1049h"))

    // Mouse tracking
    // ?1000 — basic X10 button press/release
    // ?1003 — any-event (reports all motion too)
    // ?1006 — SGR extended encoding (coordinates > 223, release distinction)
    DECL_ANSI_CMD(enable_mouse_button,    _csi("?1000h"))
    DECL_ANSI_CMD(disable_mouse_button,   _csi("?1000l"))
    DECL_ANSI_CMD(enable_mouse_any,       _csi("?1003h"))
    DECL_ANSI_CMD(disable_mouse_any,      _csi("?1003l"))
    DECL_ANSI_CMD(enable_mouse_sgr,       _csi("?1006h"))
    DECL_ANSI_CMD(disable_mouse_sgr,      _csi("?1006l"))

# define DECL_ANSI_SCREEN_MODE_CMD(name, code) \
    DECL_ANSI_CMD(set_screen_mode_##name, CSI "=" code "h") \
    DECL_ANSI_CMD(reset_screen_mode_##name, CSI "=" code "l") \

    DECL_ANSI_SCREEN_MODE_CMD(40x25_mono_text, "0")
    DECL_ANSI_SCREEN_MODE_CMD(40x25_color_text, "1")
    DECL_ANSI_SCREEN_MODE_CMD(80x25_mono_text, "2")
    DECL_ANSI_SCREEN_MODE_CMD(80x25_color_text, "3")
    DECL_ANSI_SCREEN_MODE_CMD(320x200_4color_graph, "4")
    DECL_ANSI_SCREEN_MODE_CMD(320x200_mono_graph, "5")
    DECL_ANSI_SCREEN_MODE_CMD(640x200_mono_graph, "6")
    DECL_ANSI_SCREEN_MODE_CMD(line_wrap, "7")
    DECL_ANSI_SCREEN_MODE_CMD(320x200_color_graph, "13")
    DECL_ANSI_SCREEN_MODE_CMD(640x200_color_graph, "14")
    DECL_ANSI_SCREEN_MODE_CMD(640x350_mono_graph, "15")
    DECL_ANSI_SCREEN_MODE_CMD(640x350_16color_graph, "16")
    DECL_ANSI_SCREEN_MODE_CMD(640x480_mono_graph, "17")
    DECL_ANSI_SCREEN_MODE_CMD(640x480_16color_graph, "18")
    DECL_ANSI_SCREEN_MODE_CMD(320x200_256color_graph, "19")

}

#endif //ANSI_CODES_P_HPP

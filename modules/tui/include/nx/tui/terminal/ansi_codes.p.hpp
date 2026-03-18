//
// Created by nexie on 18.03.2026.
//

#ifndef ANSI_CODES_P_HPP
#define ANSI_CODES_P_HPP

#include <string>
#include <nx/macro/util/platform.hpp>
#include <fmt/format.h>

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#escape

namespace nx::tui::ansi {
    constexpr char TerminalBell = '\a';
    constexpr char Backspace = '\b';
    constexpr char HorizontalTab = '\t';
    constexpr char NewLine = '\n';
    constexpr char VerticalTab = '\v';
    constexpr char NewPage = '\f';
    constexpr char Return = '\r';
    // constexpr char Escape = '\e';

# define ESC "\x1B"
    // # define CSI "\x9B"
# define CSI ESC"["
# define DCS "\x90"
# define OSC "\x9D"

# define _ansi(esc, code) fprintf(stdout, esc code)
# define _ansi_1(esc, p, code) fprintf(stdout, esc "%i" code, p)
# define _ansi_2(esc, p1, p2, code) fprintf(stdout, esc "%i;%i" code, p1, p2)

# define _csi(code) _ansi(CSI, code)
# define _csi_1(p, code) _ansi_1(CSI, p, code)
# define _csi_2(p1, p2, code) _ansi_2(CSI, p1, p2, code)

# define CSTR void inline
# define ISTR void inline

    CSTR CursorHome() { _csi("H"); }
    ISTR CursorPos(int row, int col) { _csi_2(row, col, "H"); }
    ISTR CursorUp(int rows) { _csi_1(rows, "A"); }
    ISTR CursorDown(int rows) { _csi_1(rows, "B"); }
    ISTR CursorRight(int cols) { _csi_1(cols, "C"); }
    ISTR CursorLeft(int cols) {  _csi_1(cols, "D"); }
    ISTR CursorNextBegin(int rows) {  _csi_1(rows, "E"); }
    ISTR CursorPrevBegin(int rows) {  _csi_1(rows, "F"); }
    ISTR CursorToColumn(int col) {  _csi_1(col, "G"); }
    CSTR RequestCursorPos() { _csi("6n"); }
    CSTR ScrollUp() { _ansi(ESC, "M"); }
    CSTR SaveCursor() { _ansi(ESC, "7"); }
    CSTR RestoreCursor() { _ansi(ESC, "8"); }


    CSTR EraseInDisplay() {_csi("J");}
    CSTR EraseToScreenEnd() {_csi("0J");}
    CSTR EraseToScreenBegin() {_csi("1J");}
    CSTR EraseScreen() {_csi("2J");}
    CSTR EraseSavedLines() {_csi("3J");}
    CSTR EraseInLine() {_csi("K");}
    CSTR EraseToLineEnd() {_csi("0K");}
    CSTR EraseToLineStart() {_csi("1K");}
    CSTR EraseLine() {_csi("2K");}


    CSTR ResetAll() { _csi("0m"); }
    CSTR SetBold() { _csi("1m"); }
    CSTR SetDim() { _csi("2m"); }
    CSTR SetItalic() { _csi("3m"); }
    CSTR SetUnderline() { _csi("4m"); }
    CSTR SetBlinking() { _csi("5m"); }
    CSTR SetReverse() { _csi("7m"); }
    CSTR SetHidden() { _csi("8m"); }
    CSTR SetStrikeThrough() { _csi("9m"); }


    CSTR ResetBold()  { _csi("22m"); }
    CSTR ResetDim()  { _csi("22m"); }
    CSTR ResetItalic()  { _csi("23m"); }
    CSTR ResetUnderline()  { _csi("24m"); }
    CSTR ResetBlinking()  { _csi("25m"); }
    CSTR ResetReverse()  { _csi("27m"); }
    CSTR ResetHidden()  { _csi("28m"); }
    CSTR ResetStrikeThrough()  { _csi("29m"); }


    CSTR SetCursorInvisible() { _csi("?25l"); }
    CSTR SetCursorVisible() { _csi("?25h"); }
    CSTR RestoreScreen() { _csi("?47l"); }
    CSTR SaveScreen() { _csi("?47h"); }
    CSTR DisableAltBuffer() { _csi("?1049l"); }
    CSTR EnableAltBuffer() { _csi("?1049h"); }


# define SCREEN_MODE(name, code) \

    // // CSTR SetScreenMode_##name() CSI"=" code "h";\
    // CSTR ResetScreenMode_##name = CSI"=" code "l";\


    SCREEN_MODE(40x25_MonoText, "0")
    SCREEN_MODE(40x25_ColorText, "1")
    SCREEN_MODE(80x25_MonoText, "2")
    SCREEN_MODE(80x25_ColorText, "3")
    SCREEN_MODE(320x200_4ColorGraph, "4")
    SCREEN_MODE(320x200_MonoGraph, "5")
    SCREEN_MODE(640x200_MonoGraph, "6")
    SCREEN_MODE(LineWrap, "7")
    SCREEN_MODE(320x200_ColorGraph, "13")
    SCREEN_MODE(640x200_ColorGraph, "14")
    SCREEN_MODE(640x350_MonoGraph, "15")
    SCREEN_MODE(640x350_16ColorGraph, "16")
    SCREEN_MODE(640x480_MonoGraph, "17")
    SCREEN_MODE(640x480_16ColorGraph, "18")
    SCREEN_MODE(320_200_256ColorGraph, "19")

}

#endif //ANSI_CODES_P_HPP

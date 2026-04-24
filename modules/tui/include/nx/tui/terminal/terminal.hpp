//
// Created by nexie on 03.03.2026.
//

#ifndef NX_TERMINAL_HPP
#define NX_TERMINAL_HPP

#include <nx/core.hpp>
#include <nx/tui/types/size.hpp>
#include <nx/tui/types/color.hpp>

#include "nx/tui/graphics/pixel.hpp"
#include "nx/tui/types/point.hpp"

namespace nx::tui
{
    class Terminal
    {
        inline static FILE * ostream_ = stderr;
    public:

        enum class Mode
        {
            Text_Mono_40x25,
            Text_Color_40x25,
            Text_Mono_80x25,
            Text_Color_80x25,
            Graph_4_Color_320x200,
            Graph_Mono_320x200,
            Graph_Mono_640x200,
            Graph_Color_320x200,
            Graph_Color_640x200,
            Graph_Mono_640x350,
            Graph_16_Color_640x350,
            Graph_Mono_640x480,
            Graph_16_Color_640x480,
            Graph_256_Color_320x200
        };

        template <typename... Args>
        static void Print(fmt::format_string<Args...> format, Args&&... args)
        {
            fmt::print(ostream_, format, std::forward<Args>(args)...);
        }

        static void Print(std::string_view text)
        {
            fmt::print(ostream_, "{}", text);
        }

        static void Print(char ch)
        {
            fmt::print(ostream_, "{}", ch);
        }

        static WindowSize
        GetWindowSize();

        static ColorType
        GetColorSupport();

        static void
        SetColorSupport(ColorType type);

        static void
        SetOutput(FILE * stream);

        [[nodiscard]] static FILE *
        GetOutputStream ();

        static void
        MoveCursorHome ();

        static void
        MoveCursor (int rows, int columns);

        static void
        MoveCursorUp (int rows);

        static void
        MoveCursorDown (int rows);

        static void
        MoveCursorRight (int columns);

        static void
        MoveCursorLeft (int columns);

        static void
        MoveCursorNextLineBegin (int lines);

        static void
        MoveCursorPrevLineBegin (int lines);

        static void
        MoveCursorToColumn (int column);

        [[nodiscard]] static Point<int>
        GetCursorPos ();

        static void
        ScrollUp ();

        static void
        SaveCursor ();

        static void
        RestoreCursor ();

        static void
        EraseInDisplay ();

        static void
        EraseToScreenEnd ();

        static void
        EraseToScreenBegin ();

        static void
        EraseScreen ();

        static void
        EraseSavedLines ();

        static void
        EraseInLine ();

        static void
        EraseToLineEnd ();

        static void
        EraseToLineBegin ();

        static void
        EraseLine ();

        static void
        EnableDim (bool enable);

        static void
        EnableItalic (bool enable);

        static void
        EnableUnderline (bool enable);

        static void
        EnableBlinking (bool enable);

        static void
        EnableInverse (bool enable);

        static void
        EnableHidden (bool enable);

        static void
        EnableLineWrap (bool enable);

        static void
        EnableStrikeThrough (bool enable);

        static void
        SetPixelStyle (PixelStyle style);

        static void
        ResetPixelStyle (PixelStyle style);

        static void
        SetColor (Color color);

        static void
        SetBackgroundColor (Color color);

        static void
        SetCursorVisible (bool visible);

        static void
        SaveScreen ();

        static void
        RestoreScreen ();

        static void
        EnableAltBuffer ();

        static void
        DisableAltBuffer ();

        static void
        SetScreenMode (Mode mode);

        static void
        ResetScreenMode (Mode mode);

        static void
        EnableRawMode();

        static void
        DisableRawMode();

        static void
        EnableMouseTracking();

        static void
        DisableMouseTracking();
    };
}

#endif //NX_TERMINAL_HPP
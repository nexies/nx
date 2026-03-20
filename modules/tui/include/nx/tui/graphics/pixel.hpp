//
// Created by nexie on 17.03.2026.
//

#ifndef NX_TUI_PIXEL_HPP
#define NX_TUI_PIXEL_HPP

#include <string>
#include <nx/tui/types/color.hpp>



namespace nx::tui
{

    enum PixelStyleOptions : uint16_t
    {
        None                = 0x0000'0000,
        Blink               = 0x0000'0001,
        Dim                 = 0x0000'0002,
        Italic              = 0x0000'0004,
        Inverted            = 0x0000'0008,
        Underlined          = 0x0000'0010,
        UnderlinedDouble    = 0x0000'0020,
        StrikeThrough       = 0x0000'0040,
        AutoMerge           = 0x0000'0080,
    };

    NX_FLAGS(PixelStyleOptions, PixelStyle);

    struct Pixel
    {
        using character_type = std::string;

        Pixel ()
        : style { PixelStyleOptions::None }
        , character { " " }
        { }

        character_type character = " ";
        Color foreground_color = Color::Default;
        Color background_color = Color::Default;
        PixelStyle style;
    };
}

#endif //NX_TUI_PIXEL_HPP
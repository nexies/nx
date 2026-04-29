#pragma once

#include <string>
#include <nx/common/types/enum_flags.hpp>
#include <nx/tui/types/color.hpp>

namespace nx::tui {

    enum class pixel_style_flag : uint16_t {
        none              = 0x0000,
        blink             = 0x0001,
        dim               = 0x0002,
        italic            = 0x0004,
        inverted          = 0x0008,
        underline         = 0x0010,
        underline_double  = 0x0020,
        strike_through    = 0x0040,
        auto_merge        = 0x0080,
        bold              = 0x0100,
    };

    NX_FLAGS(pixel_style_flag, pixel_style)

    struct pixel
    {
        using character_type = std::string;

        character_type character         = " ";
        color          foreground_color  = color::default_color;
        color          background_color  = color::default_color;
        pixel_style    style             = pixel_style_flag::none;
    };

} // namespace nx::tui

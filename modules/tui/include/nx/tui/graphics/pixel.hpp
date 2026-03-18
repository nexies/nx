//
// Created by nexie on 17.03.2026.
//

#ifndef NX_TUI_PIXEL_HPP
#define NX_TUI_PIXEL_HPP

#include <string>
#include <nx/tui/types/color.hpp>

namespace nx::tui
{
    struct Pixel
    {
        using character_type = std::string;

        bool blink : 1;
        bool bold : 1;
        bool dim : 1;
        bool italic : 1;
        bool inverted : 1;
        bool underlined : 1;
        bool underlined_double : 1;
        bool strikethrough : 1;
        bool automerge : 1;
        character_type character = " ";

        Pixel ()
        : blink { false }
        , bold { false }
        , dim { false }
        , italic { false }
        , inverted { false }
        , underlined { false }
        , underlined_double { false }
        , strikethrough { false }
        , automerge { false }
        , character { " " }
        { }

        Color foreground_color = Color::Default;
        Color background_color = Color::Default;
    };
}

#endif //NX_TUI_PIXEL_HPP
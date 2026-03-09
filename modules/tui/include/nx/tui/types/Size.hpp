//
// Created by nexie on 09.03.2026.
//

#ifndef NX_TUI_SIZE_HPP
#define NX_TUI_SIZE_HPP

namespace nx::tui {

    template <typename Type = int>
    struct Size {
        Type height;
        Type width;
    };

    struct WindowSize {
        Size<int> chars;
        Size<int> pixels;
    };
}

#endif //NX_TUI_SIZE_HPP

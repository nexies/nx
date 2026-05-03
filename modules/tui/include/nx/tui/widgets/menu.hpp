//
// Created by nexie on 03.05.2026.
//

#ifndef NX_TUI_MENU_HPP
#define NX_TUI_MENU_HPP

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/layout/box.hpp>

namespace nx::tui {
    class menu_option;

    class menu : public widget {
        std::vector<menu_option *> options_;


    public:
        void insert_option(int idx, const std::string & option);

    };


    class menu_option : public nx::core::object {
        std::string text_;
    public:
        void set_text(const std::string& text);
        NX_NODISCARD std::string
        text () const;
    };
}

#endif //NX_TUI_MENU_HPP

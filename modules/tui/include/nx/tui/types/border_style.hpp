#pragma once

namespace nx::tui {

enum class border_style {
    none,
    single,   // ─ │ ┌ ┐ └ ┘
    double_,  // ═ ║ ╔ ╗ ╚ ╝
    rounded,  // ─ │ ╭ ╮ ╰ ╯
    thick,    // ━ ┃ ┏ ┓ ┗ ┛
    dashed,   // ╌ ╎ ┌ ┐ └ ┘
};

} // namespace nx::tui

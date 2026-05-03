#pragma once

#include <cstdint>

namespace nx::tui {
    // ── size_policy ───────────────────────────────────────────────────────────────
    //
    // Describes how a widget should be sized along one axis by a layout manager.
    //
    //  fixed      — size_hint() is exact; the layout will not change this dimension.
    //  preferred  — size_hint() is preferred; the layout may shrink or grow freely.
    //  expanding  — widget wants as much space as possible; when multiple expanding
    //               siblings compete, space is divided by their stretch_factor().

    enum class size_policy : uint8_t {
        fixed,
        preferred,
        expanding,
    };
} // namespace nx::tui

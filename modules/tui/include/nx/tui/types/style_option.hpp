#pragma once

#include <nx/tui/types/color.hpp>
#include <nx/tui/graphics/pixel.hpp>

#include <optional>

namespace nx::tui {

// ── style_option ──────────────────────────────────────────────────────────────
//
// A composable, partial set of visual attributes.
// Any field left as std::nullopt is "not set" — it does not override anything
// when styles are merged.
//
// Merge semantics (operator|):
//   right-hand side fields take precedence over left-hand side when set.
//
// Typical usage:
//   widget->set_style(fg(color::white) | bg(color::blue));
//   painter.apply_style(fg(color::cyan_bright) | decoration(pixel_style_flag::italic));

struct style_option {
    std::optional<color>       foreground;
    std::optional<color>       background;
    std::optional<pixel_style> decorations;

    // Merge: any field present in rhs overrides the corresponding lhs field.
    [[nodiscard]] friend style_option
    operator|(style_option lhs, const style_option & rhs) noexcept
    {
        if (rhs.foreground)  lhs.foreground  = rhs.foreground;
        if (rhs.background)  lhs.background  = rhs.background;
        if (rhs.decorations) lhs.decorations = rhs.decorations;
        return lhs;
    }

    style_option & operator|=(const style_option & rhs) noexcept
    {
        return *this = (*this | rhs);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return !foreground && !background && !decorations;
    }
};

// ── Factory helpers ───────────────────────────────────────────────────────────

/// Returns a style_option with only the foreground color set.
[[nodiscard]] inline style_option fg(color c) noexcept
{
    style_option s;
    s.foreground = c;
    return s;
}

/// Returns a style_option with only the background color set.
[[nodiscard]] inline style_option bg(color c) noexcept
{
    style_option s;
    s.background = c;
    return s;
}

/// Returns a style_option with only the text decorations set.
[[nodiscard]] inline style_option decoration(pixel_style d) noexcept
{
    style_option s;
    s.decorations = d;
    return s;
}

// ── Decoration shortcuts ──────────────────────────────────────────────────────

[[nodiscard]] inline style_option italic() noexcept
{ return decoration(pixel_style_flag::italic); }

[[nodiscard]] inline style_option underline() noexcept
{ return decoration(pixel_style_flag::underline); }

[[nodiscard]] inline style_option inverted() noexcept
{ return decoration(pixel_style_flag::inverted); }

[[nodiscard]] inline style_option dim() noexcept
{ return decoration(pixel_style_flag::dim); }

} // namespace nx::tui

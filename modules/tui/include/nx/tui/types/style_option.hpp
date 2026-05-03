#pragma once

#include <nx/tui/types/color.hpp>
#include <nx/tui/graphics/pixel.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace nx::tui {
    // Forward declaration — full definition in style_modifier.hpp.
    class style_modifier;

    // ── style_option ──────────────────────────────────────────────────────────────
    //
    // A composable, partial set of visual attributes.
    //
    // Fields left as std::nullopt are "not set" — they do not override anything
    // when styles are merged.  Modifiers are positional transforms applied at
    // paint time; they are appended (not replaced) when styles are merged.
    //
    // Merge semantics (operator|):
    //   - Static fields: right-hand side takes precedence when set.
    //   - Modifiers: right-hand side modifiers are appended after left-hand side.
    //
    // Usage:
    //   widget->set_style(fg(color::white) | h_brightness(target::fg, 1.0f, 0.4f));

    struct style_option {
        std::optional<color> foreground;
        std::optional<color> background;
        std::optional<pixel_style> decorations;

        // Position-aware modifiers — run in order at paint time.
        // Shared ownership allows cheap copying of style_option.
        std::vector<std::shared_ptr<style_modifier> > modifiers;

        // ── Merge ─────────────────────────────────────────────────────────────────

        NX_NODISCARD friend style_option
        operator|(style_option lhs, const style_option &rhs) noexcept {
            if (rhs.foreground) lhs.foreground = rhs.foreground;
            if (rhs.background) lhs.background = rhs.background;
            if (rhs.decorations) lhs.decorations = rhs.decorations;
            lhs.modifiers.insert(lhs.modifiers.end(),
                                 rhs.modifiers.begin(), rhs.modifiers.end());
            return lhs;
        }

        style_option &
        operator|=(const style_option &rhs) noexcept {
            return *this = (*this | rhs);
        }

        NX_NODISCARD bool
        empty() const noexcept {
            return !foreground && !background && !decorations && modifiers.empty();
        }
    };

    // ── Factory helpers ───────────────────────────────────────────────────────────

    NX_NODISCARD inline style_option
    fg(color c) noexcept {
        style_option s;
        s.foreground = c;
        return s;
    }

    NX_NODISCARD inline style_option
    bg(color c) noexcept {
        style_option s;
        s.background = c;
        return s;
    }

    NX_NODISCARD inline style_option
    decoration(pixel_style d) noexcept {
        style_option s;
        s.decorations = d;
        return s;
    }


    NX_NODISCARD inline style_option
    bold() noexcept { return decoration(pixel_style_flag::bold); }

    NX_NODISCARD inline style_option
    italic() noexcept { return decoration(pixel_style_flag::italic); }

    NX_NODISCARD inline style_option
    underline() noexcept { return decoration(pixel_style_flag::underline); }

    NX_NODISCARD inline style_option
    inverted() noexcept { return decoration(pixel_style_flag::inverted); }

    NX_NODISCARD inline style_option
    dim() noexcept { return decoration(pixel_style_flag::dim); }
} // namespace nx::tui

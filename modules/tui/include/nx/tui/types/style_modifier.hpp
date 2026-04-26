#pragma once

#include <nx/tui/types/style_option.hpp>

#include <memory>

namespace nx::tui {

// ── Enums ─────────────────────────────────────────────────────────────────────

enum class axis   { horizontal, vertical };
enum class target { fg, bg, both };

// ── style_modifier ────────────────────────────────────────────────────────────
//
// Abstract base for position-aware style transforms.
//
// transform() receives the style accumulated so far for this cell
// (after static fields and all preceding modifiers) and returns the modified
// version.  col/row are 0-based relative to the widget's clip rect.
// width/height are the clip rect dimensions — use them for normalisation.
//
// Modifiers are stored inside style_option as shared_ptr, so style_option
// remains cheaply copyable and requires no manual lifetime management.

class style_modifier {
public:
    virtual ~style_modifier() = default;

    virtual style_option transform(style_option   base,
                                   int col,  int row,
                                   int width, int height) const = 0;
};

// ── gradient_modifier ─────────────────────────────────────────────────────────
//
// Replaces the target channel(s) with an interpolated color.
// Acts as a setter — the incoming color is ignored.

class gradient_modifier : public style_modifier {
    axis   axis_;
    target target_;
    color  from_, to_;

public:
    gradient_modifier(axis a, target t, color from, color to)
        : axis_(a), target_(t), from_(from), to_(to) {}

    style_option transform(style_option base,
                           int col, int row,
                           int w, int h) const override
    {
        const float t = (axis_ == axis::horizontal)
            ? (w > 1 ? float(col) / float(w - 1) : 0.f)
            : (h > 1 ? float(row) / float(h - 1) : 0.f);

        const color c = color::interpolate(t, from_, to_);

        if (target_ == target::fg   || target_ == target::both) base.foreground = c;
        if (target_ == target::bg   || target_ == target::both) base.background = c;
        return base;
    }
};

// ── brightness_modifier ───────────────────────────────────────────────────────
//
// Multiplies the RGB of the target channel(s) by a factor.
// Acts as a modifier — the incoming color is scaled, not replaced.
// When from == to the factor is constant (no gradient).

class brightness_modifier : public style_modifier {
    axis   axis_;
    target target_;
    float  from_, to_;

public:
    // Static brightness (no gradient).
    brightness_modifier(target t, float value)
        : axis_(axis::horizontal), target_(t), from_(value), to_(value) {}

    // Gradient brightness along an axis.
    brightness_modifier(axis a, target t, float from, float to)
        : axis_(a), target_(t), from_(from), to_(to) {}

    style_option transform(style_option base,
                           int col, int row,
                           int w, int h) const override
    {
        const float t = (from_ == to_) ? 0.f
            : (axis_ == axis::horizontal)
                ? (w > 1 ? float(col) / float(w - 1) : 0.f)
                : (h > 1 ? float(row) / float(h - 1) : 0.f);

        const float factor = from_ + (to_ - from_) * t;

        if (target_ == target::fg || target_ == target::both)
            if (base.foreground)
                base.foreground = color::scale(*base.foreground, factor);

        if (target_ == target::bg || target_ == target::both)
            if (base.background)
                base.background = color::scale(*base.background, factor);

        return base;
    }
};

// ── Factory helpers ───────────────────────────────────────────────────────────
//
// All functions return style_option containing the modifier.
// Compose freely with | just like fg() / bg() / bold().
//
// Three levels of verbosity — pick what reads best in context:
//
//   gradient(axis::h, target::fg, from, to)   — explicit
//   h_gradient(target::fg, from, to)           — axis in name
//   h_gradient_fg(from, to)                    — axis + target in name

namespace detail {

inline style_option make_modifier(std::shared_ptr<style_modifier> m)
{
    style_option s;
    s.modifiers.push_back(std::move(m));
    return s;
}

} // namespace detail

// ── gradient ──────────────────────────────────────────────────────────────────

[[nodiscard]] inline style_option
gradient(axis a, target t, color from, color to)
{
    return detail::make_modifier(
        std::make_shared<gradient_modifier>(a, t, from, to));
}

[[nodiscard]] inline style_option h_gradient(target t, color from, color to)
{ return gradient(axis::horizontal, t, from, to); }

[[nodiscard]] inline style_option v_gradient(target t, color from, color to)
{ return gradient(axis::vertical,   t, from, to); }

[[nodiscard]] inline style_option h_gradient_fg(color from, color to)
{ return gradient(axis::horizontal, target::fg,   from, to); }

[[nodiscard]] inline style_option h_gradient_bg(color from, color to)
{ return gradient(axis::horizontal, target::bg,   from, to); }

[[nodiscard]] inline style_option v_gradient_fg(color from, color to)
{ return gradient(axis::vertical,   target::fg,   from, to); }

[[nodiscard]] inline style_option v_gradient_bg(color from, color to)
{ return gradient(axis::vertical,   target::bg,   from, to); }

// ── brightness ────────────────────────────────────────────────────────────────

// Gradient brightness.
[[nodiscard]] inline style_option
brightness(axis a, target t, float from, float to)
{
    return detail::make_modifier(
        std::make_shared<brightness_modifier>(a, t, from, to));
}

// Static brightness (single value, no gradient).
[[nodiscard]] inline style_option brightness(target t, float value)
{
    return detail::make_modifier(
        std::make_shared<brightness_modifier>(t, value));
}

[[nodiscard]] inline style_option h_brightness(target t, float from, float to)
{ return brightness(axis::horizontal, t, from, to); }

[[nodiscard]] inline style_option v_brightness(target t, float from, float to)
{ return brightness(axis::vertical,   t, from, to); }

[[nodiscard]] inline style_option h_brightness_fg(float from, float to)
{ return brightness(axis::horizontal, target::fg, from, to); }

[[nodiscard]] inline style_option h_brightness_bg(float from, float to)
{ return brightness(axis::horizontal, target::bg, from, to); }

[[nodiscard]] inline style_option v_brightness_fg(float from, float to)
{ return brightness(axis::vertical,   target::fg, from, to); }

[[nodiscard]] inline style_option v_brightness_bg(float from, float to)
{ return brightness(axis::vertical,   target::bg, from, to); }

// Single-value shortcuts (static, no gradient).
[[nodiscard]] inline style_option brightness_fg(float v)
{ return brightness(target::fg,   v); }

[[nodiscard]] inline style_option brightness_bg(float v)
{ return brightness(target::bg,   v); }

[[nodiscard]] inline style_option brightness_all(float v)
{ return brightness(target::both, v); }

} // namespace nx::tui

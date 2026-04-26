#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/border_style.hpp>
#include <nx/tui/types/color.hpp>

#include <memory>
#include <string>

namespace nx::tui {

// ── frame ─────────────────────────────────────────────────────────────────────
//
// Container widget that draws a border around its inner area.
//
// The inner area shrinks by 1 cell on each side (top/bottom/left/right).
// Children are positioned within that inner area by the assigned layout.
//
// Usage:
//   auto * f = new frame(parent);
//   f->set_border_style(border_style::rounded);
//   f->set_title("My Panel");
//   f->set_layout(std::make_unique<v_box_layout>());
//   new label(f);  // becomes a child inside the frame

class frame : public widget {
    border_style border_style_ = border_style::single;
    color        border_color_ = color::default_color;
    std::string  title_;

public:
    NX_OBJECT(frame)

    explicit frame(nx::core::object * parent = nullptr);

    // ── Border ────────────────────────────────────────────────────────────────

    void set_border_style(border_style s) noexcept { border_style_ = s; update(); }
    [[nodiscard]] border_style get_border_style() const noexcept { return border_style_; }

    void set_border_color(color c) noexcept { border_color_ = c; update(); }
    [[nodiscard]] color get_border_color() const noexcept { return border_color_; }

    // ── Title ─────────────────────────────────────────────────────────────────

    void set_title(std::string t) { title_ = std::move(t); update(); }
    [[nodiscard]] const std::string & title() const noexcept { return title_; }

    // ── Content layout ────────────────────────────────────────────────────────
    // Assign a layout to the inner area (border excluded).
    // Equivalent to widget::set_layout but wrapped so children are positioned
    // inside the 1-cell border margin automatically.

    void set_content_layout(std::unique_ptr<layout> l);

    // ── size_hint ─────────────────────────────────────────────────────────────
    // Adds 2 (for border) to each dimension of explicit hint or current size.

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p) override;
};

} // namespace nx::tui

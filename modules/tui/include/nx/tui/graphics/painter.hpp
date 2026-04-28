#pragma once

#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>

#include <string>

namespace nx::tui {

class painter {
public:
    using rect_type   = rect<int>;
    using size_type   = size<int>;
    using point_type  = point<int>;
    using buffer_type = display_buffer;

private:
    buffer_type &  buffer_;
    rect_type      rect_;      // local-coord space (origin + extent for modifiers)
    rect_type      clip_;      // actual write region (subset of rect_)
    style_option   style_;

    // Returns the effective style for a cell at local (col, row).
    [[nodiscard]] style_option _at(int col, int row) const noexcept;

    // Write a single character to the buffer at absolute (bx, by),
    // using the effective style computed from local (local_col, local_row).
    // Skips writes outside clip_.
    void _write(int bx, int by, int local_col, int local_row,
                const std::string & ch) const;

    [[nodiscard]] point_type _project_point(const point_type & pos) const;

public:
    explicit painter(buffer_type & buffer);
    // clip_rect = rect_ (no additional clipping)
    painter(buffer_type & buffer, rect_type render_rect);
    // render_rect: coordinate space for style modifiers (local origin).
    // clip_rect:   restricts actual writes to a sub-region of render_rect.
    painter(buffer_type & buffer, rect_type render_rect, rect_type clip_rect);

    // ── Static style ──────────────────────────────────────────────────────────

    void enable_style (pixel_style s) noexcept;
    void disable_style(pixel_style s) noexcept;
    void set_style    (pixel_style s) noexcept;

    void set_color           (const color & c) noexcept;
    void set_background_color(const color & c) noexcept;

    [[nodiscard]] color current_color() const noexcept {
        return style_.foreground.value_or(color::default_color);
    }
    [[nodiscard]] color current_background_color() const noexcept {
        return style_.background.value_or(color::default_color);
    }

    // Apply a style_option: only present fields override the current style.
    void apply_style(const style_option & s) noexcept;

    // ── Draw operations ───────────────────────────────────────────────────────

    void draw_text(const point_type & pos, const std::string & text) const;
    void draw_char(const point_type & pos, const std::string & ch)   const;

    // Fill the entire clip rect with ch using the effective per-cell style.
    void fill(const std::string & ch = " ") const;
};

} // namespace nx::tui

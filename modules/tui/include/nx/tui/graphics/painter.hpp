#pragma once

#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/types/border_style.hpp>
#include <nx/tui/types/text_align.hpp>

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

    // ── Style ─────────────────────────────────────────────────────────────────

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

    // Returns a copy of this painter with `s` merged on top of the current style.
    // The original painter is unchanged — use the returned copy for a scoped style.
    //
    // Example:
    //   p.with(fg(c) | bg(d)).draw_border(border_style::rounded);
    //   auto styled = p.with(bold()); styled.draw_text({0,0}, "hello");
    [[nodiscard]] painter with(const style_option & s) const;

    // ── Theme access ──────────────────────────────────────────────────────────
    //
    // These methods look up the current application theme.
    // They return empty / default_color when no tui_application exists.

    // Full style_option for the given theme role.
    [[nodiscard]] const style_option & theme_style(theme_role r) const noexcept;

    // Foreground color for the given theme role.
    [[nodiscard]] color theme_color(theme_role r) const noexcept;

    // Background color for the given theme role.
    [[nodiscard]] color theme_bg(theme_role r) const noexcept;

    // Apply theme style as *base defaults*: only fills in painter fields that
    // are not already set by an explicit widget style.  Call at the start of
    // on_paint() to let the theme provide fallback fg/bg/decorations.
    void apply_theme_as_base(theme_role fg_role, theme_role bg_role) noexcept;

    // ── Primitive draw ────────────────────────────────────────────────────────

    void draw_text(const point_type & pos, const std::string & text) const;
    void draw_char(const point_type & pos, const std::string & ch)   const;

    // Fill the entire clip rect with ch using the effective per-cell style.
    void fill(const std::string & ch = " ") const;

    // Clear the clip rect to background.
    //   keep_background = false (default): write " " with the current background
    //     color — fully erases previous content.
    //   keep_background = true: write " " but preserve each cell's existing
    //     background_color — useful in transparent overlay widgets that want
    //     to clear stale characters without discarding the background painted
    //     by a lower layer.
    void clear(bool keep_background = false) const;

    // ── Geometry primitives ───────────────────────────────────────────────────

    // Draw text aligned within the full width of the painter's rect at `row`.
    // Truncates to fit; row defaults to 0.
    void draw_text_aligned(const std::string & text, text_align align, int row = 0) const;

    // Fill a horizontal line at `row` with `ch` across the full clip width.
    void draw_hline(int row, const std::string & ch = "─") const;

    // Fill a vertical line at `col` with `ch` across the full clip height.
    void draw_vline(int col, const std::string & ch = "│") const;

    // Draw a box border along the painter's full rect using border_style `bs`.
    // border_style::none is a no-op.
    void draw_border(border_style bs) const;

    // Same, but render `title` on the top edge:  ┌─ Title ──┐
    // `title_style` is merged on top of the current painter style for the title text.
    void draw_border(border_style bs,
                     const std::string & title,
                     const style_option & title_style = {}) const;
};

} // namespace nx::tui

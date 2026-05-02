#pragma once

#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>
#include <nx/tui/types/theme_role.hpp>

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

    // ── Draw operations ───────────────────────────────────────────────────────

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
};

} // namespace nx::tui

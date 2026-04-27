#pragma once

#include <nx/tui/widgets/widget.hpp>

#include <optional>
#include <string>
#include <cstddef>

namespace nx::tui {

// ── line_edit ─────────────────────────────────────────────────────────────────
//
// Single-line text input widget.
//
// Key bindings:
//   printable chars  — insert at cursor
//   Backspace        — delete char before cursor
//   Delete           — delete char at cursor
//   Left / Right     — move cursor
//   Home / End       — jump to start / end
//   Enter            — emit return_pressed
//
// The view scrolls horizontally when the text is wider than the widget.
// The cursor is shown as an inverted cell when the widget has focus.

class line_edit : public widget {
    std::string text_;
    std::size_t cursor_     = 0;
    std::size_t scroll_off_ = 0;

public:
    NX_OBJECT(line_edit)

    explicit line_edit(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────
    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_SIGNAL(text_changed);
    NX_PROPERTY(text, READ text, WRITE set_text, NOTIFY text_changed)

    NX_SIGNAL(return_pressed)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)           override;
    void on_key_press(key_event & e)     override;
    void on_mouse_press(mouse_event & e) override;

private:
    // Keep scroll_off_ so that the cursor is always visible.
    // When pin_vis_col is given, the cursor is locked to that screen column
    // by adjusting scroll_off_ (used for Backspace to keep cursor in place).
    void _adjust_scroll(std::optional<std::size_t> pin_vis_col = std::nullopt);
};

} // namespace nx::tui

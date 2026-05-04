#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>
#include <vector>

namespace nx::tui {

// ── text_edit ─────────────────────────────────────────────────────────────────
//
// Multi-line UTF-8 text editor with animated cursor blink and horizontal/
// vertical scrolling.
//
// Internally stores text as a vector of lines (no trailing '\n' in each line).
// set_text / text() use '\n' as the line separator.
//
// Keyboard: arrows (with line wrapping), Home/End, PgUp/PgDn, Enter (new line),
//           Backspace / Delete.  Ctrl+A selects all (future: selection).

class text_edit : public widget {
    std::vector<std::string> lines_ { "" };

    std::size_t cursor_row_  = 0;
    std::size_t cursor_byte_ = 0;  // byte offset within lines_[cursor_row_]
    int         scroll_row_  = 0;
    int         scroll_col_  = 0;

    // Remembered visual column for vertical movement (preserved across up/down).
    std::size_t preferred_col_ = 0;
    bool        prefer_col_set_ = false;

    animated_value<float> cursor_alpha_ { this, 0.0f };

public:
    NX_OBJECT(text_edit)

    explicit text_edit(nx::core::object * parent = nullptr);

    // ── Content ───────────────────────────────────────────────────────────────

    void        set_text(std::string t);
    [[nodiscard]] std::string text() const;

    [[nodiscard]] int line_count() const noexcept { return (int)lines_.size(); }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Cursor ────────────────────────────────────────────────────────────────

    [[nodiscard]] int cursor_row()  const noexcept { return (int)cursor_row_; }
    [[nodiscard]] int cursor_col()  const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(text_changed)
    NX_SIGNAL(cursor_moved, int, int)   // row, col (grapheme column)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)             override;
    void on_key_press(key_event & e)       override;
    void on_mouse_press(mouse_event & e)   override;
    void on_focus_in()                     override;
    void on_focus_out()                    override;

private:
    // Cursor movement
    void _move_left();
    void _move_right();
    void _move_up();
    void _move_down();
    void _move_home();
    void _move_end();
    void _move_page_up();
    void _move_page_down();

    // Edit operations
    void _insert_char(char32_t cp);
    void _insert_newline();
    void _delete_backward();
    void _delete_forward();

    // Scroll & blink
    void _adjust_scroll();
    void _blink_next();

    // Helpers
    [[nodiscard]] std::size_t _cursor_vis_col() const;
};

} // namespace nx::tui

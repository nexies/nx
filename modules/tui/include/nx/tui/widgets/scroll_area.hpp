#pragma once

#include <nx/tui/widgets/widget.hpp>

#include <optional>

namespace nx::tui {

// ── scroll_area ───────────────────────────────────────────────────────────────
//
// A container that shows a scrollable viewport into a (possibly larger) child
// widget.  Children are clipped to the viewport and can be scrolled with the
// mouse wheel, arrow keys, Page Up/Down, and Home/End.
//
// Usage:
//   auto * sa = new scroll_area(parent);
//   sa->set_focus_policy(widget::focus_policy::tab_focus);
//   auto * content = new v_box(sa);  // or any widget
//   // If the content's size_hint() reports a larger size than the viewport,
//   // scrollbars appear automatically.

class scroll_area : public widget {
    int  scroll_x_  = 0;
    int  scroll_y_  = 0;
    int  content_w_ = 0;   // 0 = derive from first child's size_hint
    int  content_h_ = 0;   // 0 = derive from first child's size_hint

public:
    NX_OBJECT(scroll_area)

    explicit scroll_area(nx::core::object * parent = nullptr);

    [[nodiscard]] bool _intercepts_wheel() const noexcept override { return true; }

    // ── Explicit content size ─────────────────────────────────────────────────
    // Set to override the automatic size derived from the child's size_hint().

    void set_content_width (int w) noexcept { content_w_ = w; update(); }
    void set_content_height(int h) noexcept { content_h_ = h; update(); }

    [[nodiscard]] int content_width()  const noexcept { return content_w_; }
    [[nodiscard]] int content_height() const noexcept { return content_h_; }

    // ── Scroll position ───────────────────────────────────────────────────────

    [[nodiscard]] int scroll_x() const noexcept { return scroll_x_; }
    [[nodiscard]] int scroll_y() const noexcept { return scroll_y_; }

    void scroll_to(int x, int y);
    void scroll_by(int dx, int dy);

    NX_SIGNAL(scrolled)

    // ── Size hint ─────────────────────────────────────────────────────────────

    [[nodiscard]] size_type size_hint() const override;

protected:
    [[nodiscard]] std::optional<rect<int>>
    children_clip() const noexcept override;

    void _apply_layout() override;
    void on_paint(painter & p) override;
    void on_key_press(key_event & e) override;
    void on_wheel(mouse_event & e) override;

private:
    [[nodiscard]] int  _effective_content_w() const noexcept;
    [[nodiscard]] int  _effective_content_h() const noexcept;
    [[nodiscard]] bool _needs_v_scrollbar()   const noexcept;
    [[nodiscard]] bool _needs_h_scrollbar()   const noexcept;
    [[nodiscard]] int  _v_scroll_w()          const noexcept;
    [[nodiscard]] int  _h_scroll_h()          const noexcept;
    [[nodiscard]] int  _viewport_w()          const noexcept;
    [[nodiscard]] int  _viewport_h()          const noexcept;

    void _draw_vscrollbar(painter & p) const;
    void _draw_hscrollbar(painter & p) const;
};

} // namespace nx::tui

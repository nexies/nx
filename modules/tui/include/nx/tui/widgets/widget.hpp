#pragma once

#include <memory>
#include <vector>

#include <nx/core2/object/object.hpp>

#include <nx/tui/layouts/layout.hpp>
#include <nx/tui/types/rect.hpp>
#include <nx/tui/types/size_policy.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>

namespace nx::tui {

class painter;
class screen;

// ── widget ────────────────────────────────────────────────────────────────────
//
// Base class for all TUI widgets.
//
// Geometry:  rect<int> in parent's coordinate space.
//            (0,0) = top-left column/row of the parent.
// Painting:  override on_paint(painter &) to draw content.
//            The painter's clip rect is already set to the widget's
//            global position — draw at local (0,0) for the top-left corner.
// Events:    override on_key_press, on_mouse_press, etc.
// Layout:    set_layout() assigns a layout that positions child widgets.
//            The layout is re-applied before each render pass.

class widget : public nx::core::object
{
    friend class screen;
public:
    NX_OBJECT(widget)
    using size_type  = nx::tui::size<int>;
    using rect_type  = nx::tui::rect<int>;
    using point_type = nx::tui::point<int>;

    enum class focus_policy { no_focus, tab_focus, click_focus, strong_focus };

private:
    rect_type                geometry_     {};
    size_type                hint_         {};   // explicit size hint (0 = unset)
    bool                     visible_      = true;
    bool                     enabled_      = true;
    bool                     dirty_        = true;
    bool                     focused_      = false;
    focus_policy             focus_policy_ = focus_policy::no_focus;
    std::unique_ptr<layout>  layout_;
    style_option             style_        {};
    size_policy              v_policy_     = size_policy::preferred;
    size_policy              h_policy_     = size_policy::expanding;
    int                      stretch_      = 1;

public:

    explicit widget(object * parent = nullptr);
    ~widget() override = default;

    // ── Geometry ──────────────────────────────────────────────────────────────
    NX_PROPERTY(geometry, READ geometry, WRITE set_geometry, NOTIFY geometry_changed)
    NX_PROPERTY(size, READ size)
    NX_PROPERTY(pos, READ pos)

    void
    set_geometry(const rect_type & r);

    NX_NODISCARD rect_type
    geometry()  const noexcept { return geometry_; }

    NX_NODISCARD size_type
    size()      const noexcept { return geometry_.size(); }

    NX_NODISCARD point_type
    pos()       const noexcept { return geometry_.top_left(); }

    // ── Visibility / enabled ──────────────────────────────────────────────────
    NX_PROPERTY(visible, MEMBER visible_, READ is_visible, WRITE)
    NX_PROPERTY(enabled, MEMBER enabled_, READ is_enabled, WRITE)

    NX_NODISCARD bool
    is_visible() const noexcept { return visible_; }

    NX_NODISCARD bool
    is_enabled() const noexcept { return enabled_; }

    // ── Focus ─────────────────────────────────────────────────────────────────
    NX_PROPERTY(focus_policy, MEMBER focus_policy_,
        READ, WRITE, RESET, DEFAULT focus_policy::no_focus)

    // ── Layout ────────────────────────────────────────────────────────────────
    // Widget takes ownership of the layout.
    void    set_layout(std::unique_ptr<layout> l) { layout_ = std::move(l); }
    layout * get_layout() const noexcept           { return layout_.get(); }

    // ── Size policy ───────────────────────────────────────────────────────────
    // Controls how this widget is sized by its parent layout.

    NX_NODISCARD size_policy vertical_policy()   const noexcept { return v_policy_; }
    NX_NODISCARD size_policy horizontal_policy() const noexcept { return h_policy_; }
    NX_NODISCARD int         stretch_factor()    const noexcept { return stretch_;  }

    void set_vertical_policy  (size_policy p) noexcept { v_policy_ = p; }
    void set_horizontal_policy(size_policy p) noexcept { h_policy_ = p; }
    void set_stretch_factor   (int s)         noexcept { stretch_  = s; }

    // Convenience: set policy to fixed AND record an explicit hint dimension.
    void set_fixed_width (int w) noexcept { h_policy_ = size_policy::fixed; hint_.width  = w; }
    void set_fixed_height(int h) noexcept { v_policy_ = size_policy::fixed; hint_.height = h; }

    // Record a preferred hint without changing the policy.
    void set_hint_width (int w) noexcept { hint_.width  = w; }
    void set_hint_height(int h) noexcept { hint_.height = h; }

    // ── Child widgets ─────────────────────────────────────────────────────────

    NX_NODISCARD std::vector<widget *>
    child_widgets() const;

    // ── Size hint ─────────────────────────────────────────────────────────────
    NX_PROPERTY(size_hint, READ size_hint)

    NX_NODISCARD virtual size_type
    size_hint() const;

    // ── Style ─────────────────────────────────────────────────────────────────
    NX_PROPERTY(style, MEMBER style_, READ get_style, WRITE set_style)

    NX_NODISCARD const style_option &
    get_style() const noexcept { return style_; }

    void
    set_style(style_option s) { style_ = std::move(s); update(); }

    // ── Focus state ───────────────────────────────────────────────────────────

    NX_NODISCARD bool
    has_focus() const noexcept { return focused_; }

    // ── Repaint ───────────────────────────────────────────────────────────────

    // Mark this widget as needing a repaint on the next render pass.
    void update() noexcept { dirty_ = true; }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(geometry_changed)

protected:
    // ── Explicit hint (read by subclass size_hint() overrides) ────────────────

    NX_NODISCARD size_type explicit_hint() const noexcept { return hint_; }

    // ── Event handlers (override in subclasses) ───────────────────────────────

    virtual void on_paint(painter & p);

    virtual void on_key_press(key_event & e);
    virtual void on_key_release(key_event & e);

    virtual void on_mouse_press(mouse_event & e);
    virtual void on_mouse_release(mouse_event & e);
    virtual void on_mouse_move(mouse_event & e);
    virtual void on_wheel(mouse_event & e);

    virtual void on_resize(size_type old_size, size_type new_size);
    virtual void on_focus_in();
    virtual void on_focus_out();

private:

    NX_NODISCARD bool
    _dirty() const noexcept { return dirty_; }

    void
    _clear_dirty() noexcept { dirty_ = false; }
};

} // namespace nx::tui

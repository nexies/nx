#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/graphics/display_buffer.hpp>

namespace nx::tui {

struct key_event;
struct mouse_event;

// ── screen ────────────────────────────────────────────────────────────────────
//
// Root widget that owns the terminal display surface.
//
// Maintains a front buffer (last rendered frame) and a back buffer (next
// frame).  render() paints all widgets into the back buffer then calls
// _flush_diff() which outputs only the cells that changed and swaps buffers.
//
// Coordinate system: buffer is 0-based (col, row).  Terminal ANSI codes are
// 1-based — the conversion happens inside _flush_diff().

class screen : public widget {
    friend class tui_application;

    display_buffer back_;
    display_buffer front_;
    widget *       focus_        = nullptr;
    widget *       hovered_      = nullptr;
    bool           full_repaint_ = true; // set by resize(), cleared after first _flush_diff
    int            render_calls_ = 0;    // _render_widget calls in the last render() pass
    int            total_render_calls_ = 0;

public:
    NX_OBJECT(screen)

    explicit
    screen(nx::core::object * parent = nullptr);

    // Resize both buffers and update root geometry.
    // Call this after construction and whenever the terminal is resized.
    void
    resize(int cols, int rows);

    // Paint all widgets into the back buffer, then flush differences to the
    // terminal.
    void
    render();

    // Number of _render_widget calls in the last render() pass.
    [[nodiscard]] int
    render_calls() const noexcept { return render_calls_; }

    // True if any widget in the tree has called update() since the last render().
    // Use this in the event loop to skip render() when nothing changed.
    [[nodiscard]] bool
    is_dirty() const noexcept { return _subtree_dirty(); }

    NX_NODISCARD int
    total_render_calls() const noexcept { return total_render_calls_; }

    // ── Event dispatch ────────────────────────────────────────────────────────

    bool
    dispatch_key_press(key_event e);
    bool
    dispatch_key_release(key_event e);
    bool
    dispatch_mouse(mouse_event e);

    // ── Focus ─────────────────────────────────────────────────────────────────

    NX_SIGNAL(focused_changed)
    NX_SIGNAL(rendered)   // emitted at the end of every render() pass
    void set_focused_widget(widget * w);

    NX_PROPERTY(focused_widget,
        MEMBER focus_, READ, WRITE set_focused_widget, NOTIFY focused_changed);

protected:
    // Sizes all direct children to fill the screen so that a root layout
    // widget (v_box, h_box) naturally takes the full terminal area.
    void _apply_layout() override;
    void on_paint(painter & p) override;

private:
    // Recursively render w and its children.
    // global_x/global_y: position of w in buffer (0-based) coordinates.
    // clip: the region within which this widget may write (in buffer coords).
    void
    _render_widget(widget & w, int global_x, int global_y, rect<int> clip, bool force = false);

    // Compare back_ and front_, emit ANSI for changed cells, swap buffers.
    void
    _flush_diff();

    // Hit-test: returns the deepest visible widget under (qx, qy) in buffer
    // coords.  wx/wy are the global position of the root widget passed in.
    // out_gx/out_gy receive the global origin of the returned widget.
    widget *
    _widget_at(widget & w, int wx, int wy, int qx, int qy,
               int & out_gx, int & out_gy);
};

} // namespace nx::tui

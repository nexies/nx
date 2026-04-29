#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/terminal/terminal.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>

#include <cstdio>
#include <vector>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

screen::screen(nx::core::object * parent)
    : widget(parent)
    , back_(0, 0)
    , front_(0, 0)
{}

void screen::resize(int cols, int rows)
{
    back_.resize(rows, cols);
    front_.resize(rows, cols);
    front_.clear();       // invalidate stale data — forces full diff on next render()
    full_repaint_ = true; // tell _flush_diff to erase the physical terminal first
    set_geometry({ 0, 0, cols, rows });
}

// ── layout ────────────────────────────────────────────────────────────────────
//
// Screen gives ALL direct children the full screen geometry so that a single
// root layout widget (h_box, v_box, or similar) naturally fills the terminal.

void screen::_apply_layout()
{
    const int w = size().width;
    const int h = size().height;
    for (auto * child : child_widgets()) {
        child->set_geometry({0, 0, w, h});
    }
}

// ── render ────────────────────────────────────────────────────────────────────

void screen::render()
{
    render_calls_ = 0;
    // Seed back_ with the previous frame so that cells belonging to widgets
    // we skip this pass already contain the correct content.
    back_ = front_;
    _render_widget(*this, 0, 0, rect<int>(0, 0, back_.cols(), back_.rows()));
    _flush_diff();
    total_render_calls_ += render_calls_;
    NX_EMIT(rendered)
}

void screen::_render_widget(widget & w, int global_x, int global_y, rect<int> clip, bool force)
{
    ++render_calls_;
    if (!w.is_visible()) return;

    // Nothing in this subtree needs repainting — back_ already holds the
    // correct content for these cells (copied from front_ at render start).
    if (!force && !w._dirty() && !w._subtree_dirty()) return;

    const auto sz = w.size();
    if (sz.width <= 0 || sz.height <= 0) {
        w._clear_dirty();
        w._clear_subtree_dirty();
        return;
    }

    rect<int> widget_rect(global_x, global_y, sz.width, sz.height);
    rect<int> eff_clip = widget_rect.intersect(clip);
    if (eff_clip.empty()) {
        // Widget is outside the visible area this frame.
        // Keep dirty flags so it gets painted when it comes back into view.
        return;
    }

    // Re-apply layout and repaint when this widget itself is dirty, or when
    // forced by a dirty parent (e.g. scroll_area repositioned its content).
    // _apply_layout is only called when the widget is dirty (its own state
    // changed); force only triggers a visual repaint without re-layout.
    // force_children propagates the force flag one level down so that children
    // re-render even if their own dirty bits are clear.
    bool force_children = false;
    if (w._dirty() || force) {
        if (w._dirty()) w._apply_layout();
        painter p(back_, widget_rect, eff_clip);
        p.apply_style(w.get_style());
        p.fill(" ");
        w.on_paint(p);
        w._clear_dirty();
        force_children = true;
    }

    // Children clip = parent's eff_clip, optionally narrowed by the widget's
    // own viewport (e.g. scroll_area clips children to its visible area).
    rect<int> child_clip = eff_clip;
    if (auto cc = w.children_clip()) {
        rect<int> vp_global(global_x + cc->x(), global_y + cc->y(),
                            cc->width(), cc->height());
        child_clip = vp_global.intersect(eff_clip);
    }

    for (auto * child : w.child_widgets()) {
        if (!child->is_visible()) continue;

        const int  cx  = global_x + child->pos().x;
        const int  cy  = global_y + child->pos().y;
        const auto csz = child->size();

        // Spatial cull: skip children entirely outside the clip region.
        if (csz.width > 0 && csz.height > 0) {
            const rect<int> cr(cx, cy, csz.width, csz.height);
            if (cr.intersect(child_clip).empty()) continue;
        }

        _render_widget(*child, cx, cy, child_clip, force_children);
    }

    w._clear_subtree_dirty();
}

void screen::_flush_diff()
{
    terminal::begin_frame();

    const int cols = back_.cols();
    const int rows = back_.rows();

    // After a resize the physical terminal may contain stale content at
    // positions that our diff would otherwise skip.  Erase it first so the
    // subsequent cell-by-cell pass produces a clean frame.
    if (full_repaint_) {
        terminal::erase_screen();
        full_repaint_ = false;
    }

    // Sentinel state: start "unknown" so the first changed cell always emits
    // style/color.
    color       cur_fg    = color::default_color;
    color       cur_bg    = color::default_color;
    pixel_style cur_style = pixel_style_flag::none;
    bool        clean     = true; // true = terminal matches reset state

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const auto & bp = back_.pixel_at(col, row);
            const auto & fp = front_.pixel_at(col, row);

            // Skip unchanged cells.
            if (bp.character        == fp.character        &&
                bp.foreground_color == fp.foreground_color &&
                bp.background_color == fp.background_color &&
                bp.style            == fp.style)
                continue;

            // Always move cursor explicitly for each changed cell.
            terminal::move_cursor(row + 1, col + 1);

            // Reset all attributes if anything changed.
            const bool style_changed = (bp.style != cur_style ||
                                        bp.foreground_color != cur_fg ||
                                        bp.background_color != cur_bg);
            if (style_changed || !clean) {
                terminal::print("\033[0m");  // SGR reset
                cur_fg    = color::default_color;
                cur_bg    = color::default_color;
                cur_style = pixel_style_flag::none;
                clean     = true;
            }

            if (bp.style != pixel_style_flag::none) {
                terminal::set_pixel_style(bp.style);
                cur_style = bp.style;
                clean     = false;
            }
            if (bp.foreground_color != color::default_color) {
                terminal::set_color(bp.foreground_color);
                cur_fg = bp.foreground_color;
                clean  = false;
            }
            if (bp.background_color != color::default_color) {
                terminal::set_background_color(bp.background_color);
                cur_bg = bp.background_color;
                clean  = false;
            }

            terminal::print(bp.character);
        }
    }

    // Reset terminal state after diff so subsequent raw prints look normal.
    terminal::print("\033[0m");
    terminal::end_frame();

    // Swap buffers. back_ now holds the previous frame — it will be
    // overwritten by back_ = front_ at the start of the next render().
    std::swap(back_, front_);
}

// ── event dispatch ────────────────────────────────────────────────────────────

// Collect all tab-focusable widgets in DFS (tree) order.
static void _collect_focusable(widget & w, std::vector<widget *> & out)
{
    if (!w.is_visible()) return;
    if (w.get_focus_policy() == widget::focus_policy::tab_focus ||
        w.get_focus_policy() == widget::focus_policy::strong_focus)
    {
        out.push_back(&w);
    }
    for (auto * child : w.child_widgets()) {
        _collect_focusable(*child, out);
    }
}

bool screen::dispatch_key_press(key_event e)
{
    // Run widget-level filters on the focused widget before any processing.
    if (focus_ && focus_->_run_filters_key(e)) return false;

    // Tab / Shift+Tab — cycle focus among tab-focusable widgets.
    if (e.code == key::tab) {
        std::vector<widget *> focusable;
        _collect_focusable(*this, focusable);
        if (focusable.empty()) return false;

        const bool backward = e.modifiers.has(key_modifier::shift);
        const int  n        = static_cast<int>(focusable.size());

        // Find current focus position.
        int idx = -1;
        for (int i = 0; i < n; ++i) {
            if (focusable[i] == focus_) { idx = i; break; }
        }

        if (backward) {
            idx = (idx <= 0) ? n - 1 : idx - 1;
        } else {
            idx = (idx < 0 || idx >= n - 1) ? 0 : idx + 1;
        }

        set_focused_widget(focusable[idx]);
        return true;
    }

    if (focus_) {
        return send_event(focus_, e);
    }
    return false;
}

bool screen::dispatch_key_release(key_event e)
{
    if (focus_) {
        key_event rel = e;
        rel = key_event(e.code, e.modifiers, e.character, key_event::type_release);
        return send_event(focus_, rel);
    }
    return false;
}

bool screen::dispatch_mouse(mouse_event e)
{
    // Mouse positions from the input reader are 1-based; buffer is 0-based.
    const int qx = e.position.x - 1;
    const int qy = e.position.y - 1;

    auto * target = _widget_at(*this, 0, 0, qx, qy);

    // Fire enter/leave when the deepest widget under the cursor changes.
    if (target != hovered_) {
        if (hovered_) {
            mouse_event leave;
            leave.action   = mouse_action::leave;
            leave.position = e.position;
            send_event(hovered_, leave);
        }
        hovered_ = target;
        if (hovered_) {
            mouse_event enter;
            enter.action   = mouse_action::enter;
            enter.position = e.position;
            send_event(hovered_, enter);
        }
    }

    if (!target) return false;

    // Run widget-level filters before dispatching the mouse event.
    if (target->_run_filters_mouse(e)) return false;

    switch (e.action) {
    case mouse_action::wheel: {
        // Bubble up the parent chain until we find a widget that handles wheel.
        widget * w = target;
        while (w) {
            if (w->_intercepts_wheel()) { w->on_wheel(e); return true; }
            w = dynamic_cast<widget *>(w->parent());
        }
        return false;
    }
    case mouse_action::press:
        if (target->get_focus_policy() != widget::focus_policy::no_focus)
            set_focused_widget(target);
        return send_event(target, e);
    case mouse_action::release:
        return send_event(target, e);
    case mouse_action::move:
        return send_event(target, e);
    }
    return false;
}

// ── focus ─────────────────────────────────────────────────────────────────────

void screen::set_focused_widget(widget * w)
{
    if (focus_ == w) return;
    widget * old_focus = focus_;
    if (old_focus) old_focus->on_focus_out();
    focus_ = w;
    if (focus_) focus_->on_focus_in();

    // Mark ancestors of both the old and new focused widget dirty so that
    // containers whose appearance depends on child focus (e.g. frame title)
    // get repainted.  on_focus_in/out already propagated subtree_dirty_ up;
    // we just need dirty_ set on each ancestor so on_paint is actually called.
    auto mark_ancestors = [](widget * start) {
        if (!start) return;
        auto * p = dynamic_cast<widget *>(start->parent());
        while (p) {
            p->update();
            p = dynamic_cast<widget *>(p->parent());
        }
    };
    mark_ancestors(old_focus);
    mark_ancestors(focus_);
}

// ── hit testing ──────────────────────────────────────────────────────────────

widget * screen::_widget_at(widget & w, int wx, int wy, int qx, int qy)
{
    if (!w.is_visible()) return nullptr;

    // Check if query point is within this widget's global bounds.
    const int x2 = wx + w.size().width  - 1;
    const int y2 = wy + w.size().height - 1;
    if (qx < wx || qx > x2 || qy < wy || qy > y2) return nullptr;

    // Check children in reverse order (last-painted = top-most).
    auto children = w.child_widgets();
    for (int i = static_cast<int>(children.size()) - 1; i >= 0; --i) {
        auto * child  = children[i];
        int    cx     = wx + child->pos().x;
        int    cy     = wy + child->pos().y;
        if (auto * found = _widget_at(*child, cx, cy, qx, qy)) {
            return found;
        }
    }

    return &w;
}

} // namespace nx::tui

#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/layouts/layout.hpp>
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

// ── render ────────────────────────────────────────────────────────────────────

void screen::render()
{
    back_.clear();
    _render_widget(*this, get_style(), 0, 0);
    _flush_diff();
}

void screen::_render_widget(widget & w, style_option style, int global_x, int global_y)
{
    if (!w.is_visible()) return;

    // Apply layout to reposition children before painting.
    if (auto * l = w.get_layout()) {
        l->apply(w);
    }

    const auto sz = w.size();
    if (sz.width <= 0 || sz.height <= 0) return;

    // Paint this widget using a painter clipped to its global rect.
    {
        rect<int> clip(global_x, global_y, sz.width, sz.height);
        painter   p(back_, clip);
        style |= w.get_style();
        p.apply_style(style); // pre-configure painter with widget's base style
        w.on_paint(p);
        w._clear_dirty();
    }

    // Recurse into children (front-to-back: later children paint on top).
    for (auto * child : w.child_widgets()) {
        _render_widget(*child, style, global_x + child->pos().x, global_y + child->pos().y);
    }
}

void screen::_flush_diff()
{
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
    fflush(terminal::get_output_stream());

    // Swap buffers; clear back for the next frame.
    std::swap(back_, front_);
    back_.clear();
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

void screen::dispatch_key_press(key_event e)
{
    // Tab / Shift+Tab — cycle focus among tab-focusable widgets.
    if (e.code == key::tab) {
        std::vector<widget *> focusable;
        _collect_focusable(*this, focusable);
        if (focusable.empty()) return;

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
        return;
    }

    if (focus_) {
        focus_->on_key_press(e);
    }
}

void screen::dispatch_key_release(key_event e)
{
    if (focus_) {
        focus_->on_key_release(e);
    }
}

void screen::dispatch_mouse(mouse_event e)
{
    // Mouse positions from the input reader are 1-based; buffer is 0-based.
    const int qx = e.position.x - 1;
    const int qy = e.position.y - 1;

    auto * target = _widget_at(*this, 0, 0, qx, qy);
    if (!target) return;

    switch (e.action) {
    case mouse_action::press:
        if (e.button == mouse_button::wheel_up || e.button == mouse_button::wheel_down) {
            target->on_wheel(e);
        } else {
            if (target->get_focus_policy() != widget::focus_policy::no_focus) {
                set_focused_widget(target);
            }
            target->on_mouse_press(e);
        }
        break;
    case mouse_action::release:
        target->on_mouse_release(e);
        break;
    case mouse_action::move:
        target->on_mouse_move(e);
        break;
    }
}

// ── focus ─────────────────────────────────────────────────────────────────────

void screen::set_focused_widget(widget * w)
{
    if (focus_ == w) return;
    if (focus_) focus_->on_focus_out();
    focus_ = w;
    if (focus_) focus_->on_focus_in();
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

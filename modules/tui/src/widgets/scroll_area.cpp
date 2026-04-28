#include <nx/tui/widgets/scroll_area.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/input/key.hpp>

#include <algorithm>

namespace nx::tui {

scroll_area::scroll_area(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
}

// ── geometry helpers ──────────────────────────────────────────────────────────

int scroll_area::_effective_content_w() const noexcept
{
    if (content_w_ > 0) return content_w_;
    auto children = child_widgets();
    if (!children.empty()) {
        const int sh = children.front()->size_hint().width;
        if (sh > 0) return sh;
    }
    return size().width;
}

int scroll_area::_effective_content_h() const noexcept
{
    if (content_h_ > 0) return content_h_;
    auto children = child_widgets();
    if (!children.empty()) {
        const int sh = children.front()->size_hint().height;
        if (sh > 0) return sh;
    }
    return size().height;
}

bool scroll_area::_needs_v_scrollbar() const noexcept
{
    return _effective_content_h() > size().height;
}

bool scroll_area::_needs_h_scrollbar() const noexcept
{
    return _effective_content_w() > size().width;
}

int scroll_area::_v_scroll_w() const noexcept { return _needs_v_scrollbar() ? 1 : 0; }
int scroll_area::_h_scroll_h() const noexcept { return _needs_h_scrollbar() ? 1 : 0; }

int scroll_area::_viewport_w() const noexcept
{
    return std::max(0, size().width  - _v_scroll_w());
}

int scroll_area::_viewport_h() const noexcept
{
    return std::max(0, size().height - _h_scroll_h());
}

// ── scroll position ───────────────────────────────────────────────────────────

void scroll_area::scroll_to(int x, int y)
{
    const int max_x = std::max(0, _effective_content_w() - _viewport_w());
    const int max_y = std::max(0, _effective_content_h() - _viewport_h());
    scroll_x_ = std::clamp(x, 0, max_x);
    scroll_y_ = std::clamp(y, 0, max_y);
    update();
    NX_EMIT(scrolled)
}

void scroll_area::scroll_by(int dx, int dy)
{
    scroll_to(scroll_x_ + dx, scroll_y_ + dy);
}

// ── size hint ─────────────────────────────────────────────────────────────────

widget::size_type scroll_area::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        h.height > 0 ? h.height : s.height,
        h.width  > 0 ? h.width  : s.width
    };
}

// ── layout ────────────────────────────────────────────────────────────────────

std::optional<rect<int>> scroll_area::children_clip() const noexcept
{
    return rect<int>(0, 0, _viewport_w(), _viewport_h());
}

void scroll_area::_apply_layout()
{
    const int vw = _viewport_w();
    const int vh = _viewport_h();
    const int cw = std::max(vw, _effective_content_w());
    const int ch = std::max(vh, _effective_content_h());

    // Re-clamp scroll (viewport may have changed due to resize).
    scroll_x_ = std::clamp(scroll_x_, 0, std::max(0, cw - vw));
    scroll_y_ = std::clamp(scroll_y_, 0, std::max(0, ch - vh));

    // Position the content at a negative offset so that the scrolled portion
    // aligns with the viewport origin.
    for (auto * child : child_widgets())
        child->set_geometry({ -scroll_x_, -scroll_y_, cw, ch });
}

// ── painting ──────────────────────────────────────────────────────────────────

void scroll_area::_draw_vscrollbar(painter & p) const
{
    const int vp_h  = _viewport_h();
    const int ct_h  = _effective_content_h();
    const int bar_x = size().width - 1;

    if (vp_h <= 0 || ct_h <= 0) return;

    // Reserve 1 cell each for ▲ and ▼ arrows (needs at least 3 rows).
    const bool arrows     = (vp_h >= 3);
    const int  track_y    = arrows ? 1 : 0;
    const int  track_len  = vp_h - (arrows ? 2 : 0);

    if (arrows) {
        p.draw_char({ bar_x, 0 },         "▲");
        p.draw_char({ bar_x, vp_h - 1 },  "▼");
    }

    if (track_len <= 0) return;

    const int thumb_len = std::max(1, track_len * vp_h / ct_h);
    const int max_scroll = ct_h - vp_h;
    const int thumb_top  = max_scroll > 0
        ? (scroll_y_ * (track_len - thumb_len)) / max_scroll
        : 0;

    for (int i = 0; i < track_len; ++i) {
        const bool in_thumb = (i >= thumb_top && i < thumb_top + thumb_len);
        p.draw_char({ bar_x, track_y + i }, in_thumb ? "█" : "▒");
    }
}

void scroll_area::_draw_hscrollbar(painter & p) const
{
    const int vp_w  = _viewport_w();
    const int ct_w  = _effective_content_w();
    const int bar_y = size().height - 1;

    if (vp_w <= 0 || ct_w <= 0) return;

    const bool arrows    = (vp_w >= 3);
    const int  track_x   = arrows ? 1 : 0;
    const int  track_len = vp_w - (arrows ? 2 : 0);

    if (arrows) {
        p.draw_char({ 0,         bar_y }, "◄");
        p.draw_char({ vp_w - 1, bar_y }, "►");
    }

    if (track_len <= 0) return;

    const int thumb_len  = std::max(1, track_len * vp_w / ct_w);
    const int max_scroll = ct_w - vp_w;
    const int thumb_left = max_scroll > 0
        ? (scroll_x_ * (track_len - thumb_len)) / max_scroll
        : 0;

    for (int i = 0; i < track_len; ++i) {
        const bool in_thumb = (i >= thumb_left && i < thumb_left + thumb_len);
        p.draw_char({ track_x + i, bar_y }, in_thumb ? "█" : "▒");
    }
}

void scroll_area::on_paint(painter & p)
{
    if (_needs_v_scrollbar()) _draw_vscrollbar(p);
    if (_needs_h_scrollbar()) _draw_hscrollbar(p);

    // Corner cell (where both scrollbars meet).
    if (_needs_v_scrollbar() && _needs_h_scrollbar())
        p.draw_char({ size().width - 1, size().height - 1 }, "┘");
}

// ── input ─────────────────────────────────────────────────────────────────────

void scroll_area::on_wheel(mouse_event & e)
{
    const int delta = (e.button == mouse_button::wheel_up) ? -3 : 3;
    scroll_by(0, delta);
}

void scroll_area::on_key_press(key_event & e)
{
    const int page = std::max(1, _viewport_h() - 1);

    switch (e.code) {
    case key::arrow_up:    scroll_by(0, -1);    break;
    case key::arrow_down:  scroll_by(0,  1);    break;
    case key::arrow_left:  scroll_by(-1, 0);    break;
    case key::arrow_right: scroll_by( 1, 0);    break;
    case key::page_up:     scroll_by(0, -page); break;
    case key::page_down:   scroll_by(0,  page); break;
    case key::home:        scroll_to(0, 0);     break;
    case key::end:
        scroll_to(0, _effective_content_h() - _viewport_h());
        break;
    default: break;
    }
}

} // namespace nx::tui

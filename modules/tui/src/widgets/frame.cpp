#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>

namespace nx::tui {

// ── frame ─────────────────────────────────────────────────────────────────────

frame::frame(nx::core::object * parent)
    : widget(parent)
{}

// Position all children inside the 1-cell border margin.
void frame::_apply_layout()
{
    const int pw = size().width;
    const int ph = size().height;
    if (pw < 3 || ph < 3) return;

    for (auto * child : child_widgets())
        child->set_geometry({1, 1, pw - 2, ph - 2});
}

widget::size_type frame::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        (h.height > 0 ? h.height : s.height),
        (h.width  > 0 ? h.width  : s.width)
    };
}

void frame::on_paint(painter & p)
{
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);

    if (!is_transparent())
        p.clear(false);

    if (border_style_ == border_style::none) {
        p.fill();
        return;
    }

    color border_c = border_color_;
    if (border_c == color::default_color) {
        border_c = has_focused_descendant()
            ? p.theme_color(theme_role::border_focus)
            : p.theme_color(theme_role::border);
    }

    auto border_p = p.with(fg(border_c));

    if (title_.empty()) {
        border_p.draw_border(border_style_);
    } else {
        style_option title_s = fg(p.current_color());
        if (has_focused_descendant()) title_s |= bold();
        border_p.draw_border(border_style_, title_, title_s);
    }
}

} // namespace nx::tui

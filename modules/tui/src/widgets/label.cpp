#include <nx/tui/widgets/label.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/detail/utf8.hpp>

namespace nx::tui {

label::label(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::no_focus);
    set_fixed_height(1);
}

widget::size_type label::size_hint() const
{
    const int w = explicit_hint().width > 0 ? explicit_hint().width : size().width;
    return { 1, w };
}

void label::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    NX_EMIT(text_changed, text_)
    update();
}

void label::set_alignment(text_align a)
{
    if (align_ == a) return;
    align_ = a;
    update();
}

void label::on_paint(painter & p)
{
    p.fill(" ");

    const int w = size().width;
    if (w <= 0 || text_.empty()) return;

    // Truncate to fit width.
    std::string visible = text_.size() > static_cast<std::size_t>(w)
        ? text_.substr(0, static_cast<std::size_t>(w))
        : text_;

    int x = 0;
    switch (align_) {
    case text_align::left:
        x = 0;
        break;
    case text_align::center:
        x = (w - static_cast<int>(visible.size())) / 2;
        break;
    case text_align::right:
        x = w - static_cast<int>(visible.size());
        break;
    }
    if (x < 0) x = 0;

    p.draw_text({ x, 0 }, visible);
}

} // namespace nx::tui

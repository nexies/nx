#include <nx/tui/widgets/label.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/common/types/utf8.hpp>

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
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill();
    if (!text_.empty())
        p.draw_text_aligned(text_, align_);
}

} // namespace nx::tui

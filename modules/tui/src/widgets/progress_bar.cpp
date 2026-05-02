#include <nx/tui/widgets/progress_bar.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>

#include <algorithm>
#include <string>

namespace nx::tui {

progress_bar::progress_bar(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::no_focus);
    set_fixed_height(1);
}

widget::size_type progress_bar::size_hint() const
{
    const auto eh = explicit_hint();
    return { 1, eh.width > 0 ? eh.width : size().width };
}

void progress_bar::set_value(float v, bool animate)
{
    v = std::clamp(v, 0.0f, 1.0f);
    if (v == target_) return;
    target_ = v;
    if (animate) value_.animate_to(v, 300, easing::ease_out);
    else         value_.set(v);
    NX_EMIT(value_changed, target_)
}

void progress_bar::on_paint(painter & p)
{
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill(" ");

    const int w = size().width;
    if (w <= 0) return;

    const float v       = std::clamp(value_.value(), 0.0f, 1.0f);
    const int   filled  = static_cast<int>(static_cast<float>(w) * v + 0.5f);
    const int   empty   = w - filled;

    const color fill_bg  = p.theme_bg(theme_role::control_active);
    const color empty_bg = p.theme_bg(theme_role::background_alt);
    const color fg       = p.theme_color(theme_role::foreground);

    // Draw filled portion
    p.set_background_color(fill_bg);
    p.set_color(fg);
    for (int x = 0; x < filled; ++x)
        p.draw_char({ x, 0 }, " ");

    // Draw empty portion
    p.set_background_color(empty_bg);
    for (int x = filled; x < filled + empty; ++x)
        p.draw_char({ x, 0 }, " ");

    // Percentage label (drawn over the bar)
    if (show_text_) {
        const int pct = static_cast<int>(v * 100.0f + 0.5f);
        const std::string label = std::to_string(pct) + "%";
        const int lx = std::max(0, (w - static_cast<int>(label.size())) / 2);
        p.set_color(fg);
        p.set_background_color(color::default_color); // let existing bg show through
        p.draw_text({ lx, 0 }, label);
    }
}

} // namespace nx::tui

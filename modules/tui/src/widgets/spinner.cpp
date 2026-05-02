#include <nx/tui/widgets/spinner.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/animator.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/animation/easing.hpp>

namespace nx::tui {

static constexpr const char * k_frames[10] = {
    "⠋","⠙","⠹","⠸","⠼","⠴","⠦","⠧","⠇","⠏"
};

spinner::spinner(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::no_focus);
    set_fixed_height(1);
}

int spinner::_current_frame() const noexcept
{
    const float v = tick_.value(); // 0.0 → 1.0
    return static_cast<int>(v * k_frame_count) % k_frame_count;
}

void spinner::_next_cycle()
{
    if (!spinning_) return;
    tick_.set(0.0f);
    tick_.animate_to(1.0f, k_cycle_ms, easing::linear);
    // Re-arm for the next loop.
    nx::core::connect(&tick_.raw(), &animator::finished,
                      this, &spinner::_next_cycle,
                      nx::core::connection_type::auto_t,
                      nx::core::connection_flag::single_shot);
}

void spinner::start()
{
    if (spinning_) return;
    spinning_ = true;
    _next_cycle();
    update();
}

void spinner::stop()
{
    spinning_ = false;
    tick_.set(0.0f); // cancels animation and unregisters from manager
    update();
}

widget::size_type spinner::size_hint() const
{
    const int label_w = label_.empty() ? 0 : 1 + static_cast<int>(label_.size());
    return { 1, 1 + label_w };
}

void spinner::on_paint(painter & p)
{
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill(" ");

    if (!spinning_) return;

    const color fg = p.theme_color(theme_role::highlight);
    p.set_color(fg);
    p.draw_char({ 0, 0 }, k_frames[_current_frame()]);

    if (!label_.empty()) {
        p.set_color(p.theme_color(theme_role::foreground));
        p.draw_text({ 2, 0 }, label_);
    }
}

} // namespace nx::tui

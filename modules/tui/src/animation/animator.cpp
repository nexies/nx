#include <nx/tui/animation/animator.hpp>
#include <nx/tui/animation/animation_manager.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/widgets/widget.hpp>

#include <algorithm>
#include <cmath>

namespace nx::tui {

animator::animator(nx::core::object * parent)
    : nx::core::object(parent)
{}

animator::~animator()
{
    _unregister();
}

void animator::animate_to(float target, int duration_ms,
                           easing_fn fn, bool proportional)
{
    from_   = value_;
    target_ = target;
    easing_ = fn ? fn : easing::linear;

    const float distance = std::abs(target_ - from_);

    // Already at target — snap and notify.
    if (distance < 1e-5f) {
        active_ = false;
        _unregister();
        NX_EMIT(finished)
        return;
    }

    int actual = duration_ms;
    if (proportional)
        actual = static_cast<int>(static_cast<float>(duration_ms) * distance);
    duration_ = std::max(1, actual);

    start_time_ = std::chrono::steady_clock::now();
    active_     = true;
    _register();
}

void animator::set_value(float v) noexcept
{
    value_  = v;
    from_   = v;
    target_ = v;
    if (active_) {
        active_ = false;
        _unregister();
    }
}

void animator::set_update(std::function<void(float)> fn)
{
    on_update_ = std::move(fn);
}

bool animator::_tick(time_point now)
{
    if (!active_) return false;

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                             now - start_time_).count();

    float t = static_cast<float>(elapsed) / static_cast<float>(duration_);
    t = std::clamp(t, 0.0f, 1.0f);

    value_ = from_ + (target_ - from_) * easing_(t);

    if (on_update_) on_update_(value_);

    // Mark parent widget dirty so the next render pass picks up the change.
    if (auto * w = dynamic_cast<widget *>(parent()))
        w->update();

    if (t >= 1.0f) {
        value_  = target_;
        active_ = false;
        NX_EMIT(finished)
        return false;
    }
    return true;
}

void animator::_register()
{
    if (auto * app = tui_application::instance())
        app->animations()._register(this);
}

void animator::_unregister()
{
    if (auto * app = tui_application::instance())
        app->animations()._unregister(this);
}

} // namespace nx::tui

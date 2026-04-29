#include <nx/tui/animation/animation_manager.hpp>
#include <nx/tui/animation/animator.hpp>
#include <nx/asio/steady_timer.hpp>
#include <nx/asio/context/io_context.hpp>

#include <algorithm>

namespace nx::tui {

animation_manager::animation_manager(nx::asio::io_context & ctx)
    : timer_(std::make_unique<nx::asio::steady_timer>(ctx))
{}

animation_manager::~animation_manager() = default;

void animation_manager::add_on_tick(std::function<void()> fn)
{
    on_tick_cbs_.push_back(std::move(fn));
}

void animation_manager::_register(animator * a)
{
    if (std::find(active_.begin(), active_.end(), a) == active_.end())
        active_.push_back(a);
    if (!ticking_) _arm();
}

void animation_manager::_unregister(animator * a)
{
    active_.erase(std::remove(active_.begin(), active_.end(), a), active_.end());
}

void animation_manager::_arm()
{
    ticking_ = true;
    timer_->async_wait(std::chrono::milliseconds(k_frame_ms),
                       [this]() { _on_timer(); });
}

void animation_manager::_on_timer()
{
    const auto now = std::chrono::steady_clock::now();

    // Snapshot the list — _tick() may call _unregister() via NX_EMIT(finished).
    const auto snapshot = active_;
    for (auto * a : snapshot)
        a->_tick(now);

    // Remove animators that finished during this tick.
    active_.erase(
        std::remove_if(active_.begin(), active_.end(),
                       [](animator * a) { return !a->is_animating(); }),
        active_.end());

    for (auto & cb : on_tick_cbs_) cb();

    if (!active_.empty())
        _arm();
    else
        ticking_ = false;
}

} // namespace nx::tui

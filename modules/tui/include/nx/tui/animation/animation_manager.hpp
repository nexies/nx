#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace nx::asio {
    class steady_timer;
    class io_context;
}

namespace nx::tui {

class animator;

// ── animation_manager ─────────────────────────────────────────────────────────
//
// Drives all active animators with a single steady_timer (~60 fps).
// The timer only runs when there is at least one active animator.
// Owned by tui_application; accessed via tui_application::animations().

class animation_manager {
    static constexpr int k_frame_ms = 16; // ~62.5 fps

    std::unique_ptr<nx::asio::steady_timer> timer_;
    std::vector<animator *>                 active_;
    bool                                         ticking_ = false;
    std::vector<std::function<void()>>           on_tick_cbs_;

public:
    explicit animation_manager(nx::asio::io_context & ctx);
    ~animation_manager();

    // Register a callback invoked after every tick.
    // Multiple callbacks are supported; they fire in registration order.
    void add_on_tick(std::function<void()> fn);

    // Called by animator when it starts/stops.
    void _register  (animator * a);
    void _unregister(animator * a);

private:
    void _arm();
    void _on_timer();
};

} // namespace nx::tui

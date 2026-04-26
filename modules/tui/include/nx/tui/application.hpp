//
// tui_application — entry point for an nx::tui-based application.
//
// Extends nx::core::application with terminal lifecycle management:
//   - Switches to alt screen buffer on exec(), restores it on exit.
//   - Handles SIGWINCH (terminal resize) and emits window_resized signal.
//
// Usage:
//   int main(int argc, char * argv[]) {
//       nx::tui::tui_application app(argc, argv);
//       // create widgets, connect signals …
//       return app.exec();
//   }
//

#pragma once

#include <nx/core2/app/application.hpp>
#include <nx/asio/signal_set.hpp>
#include <nx/tui/terminal.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/input/input_reader.hpp>
#include <nx/tui/input/event_filter.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace nx::tui {

// ──────────────────────────────────────────────────────────────────────────────
// tui_application
// ──────────────────────────────────────────────────────────────────────────────

class tui_application : public nx::core::application {
public:
    NX_OBJECT(tui_application)

    tui_application();
    explicit tui_application(int argc, char * argv[]);
    ~tui_application() override;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    // Sets up terminal (alt buffer, raw mode, cursor) before running the loop.
    // Restores the terminal state after the loop exits.
    int exec() override;

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted whenever the terminal window is resized (SIGWINCH).
    NX_SIGNAL(window_resized, window_size)

    // ── Event filters ─────────────────────────────────────────────────────────
    // App-level filters run before any event reaches the screen or default
    // application handling (e.g. Escape → quit).
    // The application does NOT take ownership; caller must manage lifetime.

    void install_event_filter(event_filter * f);
    void remove_event_filter (event_filter * f);

    // ── Screen access ─────────────────────────────────────────────────────────

    [[nodiscard]] screen * main_screen() const noexcept { return screen_.get(); }

    // ── Static access ─────────────────────────────────────────────────────────

    NX_NODISCARD static tui_application *
    instance();

protected:
    void _on_os_signal(int signum) override;

private:
    void _on_sigwinch();
    void _arm_sigwinch();
    void _on_key(key_event e);
    void _on_mouse(mouse_event e);
    void _on_window_resize(window_size ws);


    std::vector<event_filter *>           app_filters_;
    std::unique_ptr<nx::asio::signal_set> sigwinch_set_;
    std::unique_ptr<screen>               screen_;
    std::unique_ptr<input_reader>         input_reader_;

    std::optional<alt_screen_guard> alt_screen_;
    std::optional<cursor_guard>     cursor_;
    std::optional<mouse_guard>      mouse_;
    std::optional<raw_mode_guard>   raw_mode_;
};

#define nx_tui_app (::nx::tui::tui_application::instance())

} // namespace nx::tui

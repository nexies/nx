//
// application — singleton entry point for an nx::core2-based program.
//
// Usage:
//   int main(int argc, char * argv[]) {
//       nx::core::application app(argc, argv);
//       // set up objects, connect signals …
//       return app.exec();
//   }
//
// The application:
//   - Creates the main thread (local_thread for the calling OS thread)
//   - Initialises the default logger
//   - Sets up async OS signal handling (SIGINT, SIGTERM, …)
//   - Runs the main event loop when exec() is called
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/asio/signal_set.hpp>

#include <nx/core2/object.hpp>
#include <nx/core2/thread/thread.hpp>

#include <memory>
#include <string>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// application
// ──────────────────────────────────────────────────────────────────────────────

class application : public object {
public:
    application();
    explicit application(int argc, char * argv[]);
    ~application() override;

    NX_DISABLE_COPY(application)
    NX_OBJECT(application)

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    // Run the main event loop. Blocks until quit() or exit() is called.
    // Returns the exit code.
    virtual int
    exec();

    // Request a graceful shutdown with exit code 0.
    void
    quit();

    // Request a graceful shutdown with the given code.
    void
    exit(int code = 0);

    NX_NODISCARD bool
    is_running() const noexcept;

    // ── Info ──────────────────────────────────────────────────────────────────

    const std::string &
    app_name() const noexcept;

    void
    set_app_name(const std::string & name);

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted just before exec() returns.
    NX_SIGNAL(about_to_quit)

    // Emitted when an OS signal (SIGINT, SIGTERM, …) is received.
    // The integer argument is the signal number.
    NX_SIGNAL(os_signal_received, int)

    // ── Static access ─────────────────────────────────────────────────────────

    NX_NODISCARD static application *
    instance() noexcept;

protected:
    virtual void
    _on_os_signal(int signum);

    virtual void
    _print_startup_info() const;

private:
    nx::result<void>
    _init(int argc, char * argv[]);

    nx::result<void>
    _create_logger();

    nx::result<void>
    _create_main_thread();

    nx::result<void>
    _setup_os_signals();

    static void
    _os_signal_handler(nx::result<int> sig_result);

    // ── NX_SIGNAL for internal exit propagation ───────────────────────────────
    NX_SIGNAL(_do_exit, int)

    void
    _handle_exit(int code);

    static application *       instance_;
    std::string                app_name_;
    std::unique_ptr<local_thread> main_thread_;
    std::unique_ptr<nx::asio::signal_set> signal_set_;
    bool                       running_ { false };
};

} // namespace nx::core

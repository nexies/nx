#include <nx/tui/application.hpp>

#include <nx/common/platform.hpp>
#include <nx/asio/io_context.hpp>
#include <nx/core2/thread/thread.hpp>

#include <algorithm>
#include <csignal>

using namespace nx::tui;

tui_application::tui_application()
    : nx::core::application()
    , screen_(std::make_unique<screen>(this))
    , anim_manager_(std::make_unique<animation_manager>(
          nx::core::thread::current_context()))
{}

tui_application::tui_application(int argc, char * argv[])
    : nx::core::application(argc, argv)
    , screen_(std::make_unique<screen>(this))
    , anim_manager_(std::make_unique<animation_manager>(
          nx::core::thread::current_context()))
{}

tui_application::~tui_application() = default;

tui_application *
tui_application::instance()
{
    return dynamic_cast<tui_application *>(nx::core::application::instance());
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

int
tui_application::exec()
{
    _arm_sigwinch();

    alt_screen_.emplace();
    cursor_.emplace(false);
    raw_mode_.emplace();
    mouse_.emplace();

    input_reader_ = std::make_unique<tui::input_reader>(this);

    nx::core::connect(input_reader_.get(), &input_reader::key_pressed,
                      this, &tui_application::_on_key);
    nx::core::connect(input_reader_.get(), &input_reader::mouse_input,
                      this, &tui_application::_on_mouse);
#if defined(NX_OS_WINDOWS)
    nx::core::connect(input_reader_.get(), &input_reader::window_resized,
                      this, &tui_application::_on_window_resize);
#endif

    // Resize the root screen to match the current terminal dimensions.
    // (screen_ is created in the constructor; we just adapt it to the real
    //  terminal size which is now known because the alt-buffer is active.)
    {
        auto ws = terminal::get_window_size();
        screen_->resize(ws.chars.width, ws.chars.height);
    }

    // Wire resize signal → screen resize.
    nx::core::connect(this,  &tui_application::window_resized,
                      this,  &tui_application::_on_window_resize);

    // Hook animation ticks into the render loop.
    anim_manager_->add_on_tick([this]() {
        if (screen_ && screen_->is_dirty())
            screen_->render();
    });

    // Render the initial frame now — the alt buffer is already active, so
    // terminal I/O works synchronously.  Running this before the event loop
    // ensures any handlers posted before exec() (e.g. raw-terminal demos that
    // post their own draw calls) run AFTER the first widget render, not before
    // it (which would cause the full-repaint erase to wipe their output).
    if (screen_) screen_->render();

    if (input_reader_) input_reader_->start();

    const int code = nx::core::application::exec();


    nx::core::disconnect(input_reader_.get(), &input_reader::key_pressed,
                         this, &tui_application::_on_key);
    nx::core::disconnect(input_reader_.get(), &input_reader::mouse_input,
                         this, &tui_application::_on_mouse);
#if defined(NX_OS_WINDOWS)
    nx::core::disconnect(input_reader_.get(), &input_reader::window_resized,
                         this, &tui_application::_on_window_resize);
#endif

    input_reader_.reset(nullptr);
    screen_.reset();

    mouse_.reset();
    raw_mode_.reset();
    cursor_.reset();
    alt_screen_.reset();

    sigwinch_set_.reset();
    return code;
}

// ── Signal handling ───────────────────────────────────────────────────────────

void
tui_application::_arm_sigwinch()
{
#if defined(NX_POSIX)
    if (!sigwinch_set_) {
        sigwinch_set_ = std::make_unique<nx::asio::signal_set>(
            nx::core::thread::current_context());
        sigwinch_set_->add(SIGWINCH);
    }
    sigwinch_set_->async_wait([this](nx::result<int> res) {
        if (res)
            _on_sigwinch();
    });
#endif
    // Windows: no SIGWINCH equivalent; resize events are not yet implemented.
}

void
tui_application::_on_sigwinch()
{
    auto ws = terminal::get_window_size();
    NX_EMIT(window_resized, ws);
    _arm_sigwinch();
}

void
tui_application::_on_os_signal(int signum)
{
    nx::core::application::_on_os_signal(signum);
}

void
tui_application::install_event_filter(event_filter * f)
{
    if (f && std::find(app_filters_.begin(), app_filters_.end(), f) == app_filters_.end())
        app_filters_.push_back(f);
}

void
tui_application::remove_event_filter(event_filter * f)
{
    app_filters_.erase(std::remove(app_filters_.begin(), app_filters_.end(), f),
                       app_filters_.end());
}

void
tui_application::_on_key(key_event e)
{
    // App-level filters run first.
    for (auto * f : app_filters_) {
        if (f->filter_key(e)) {
            if (screen_ && screen_->is_dirty()) screen_->render();
            return;
        }
    }

    if (e.code == key::printable &&
        e.modifiers.has(key_modifier::ctrl) &&
        e.character == U'C') {
        this->quit();
        return;
    }
    if (screen_) {
        screen_->dispatch_key_press(e);
        if (screen_->is_dirty())
            screen_->render();
    }
}

void
tui_application::_on_mouse(mouse_event e)
{
    // App-level filters run first.
    for (auto * f : app_filters_) {
        if (f->filter_mouse(e)) {
            if (screen_ && screen_->is_dirty()) screen_->render();
            return;
        }
    }

    if (screen_) {
        screen_->dispatch_mouse(e);
        if (screen_->is_dirty())
            screen_->render();
    }
}

void
tui_application::_on_window_resize(window_size ws)
{
    if (screen_) {
        screen_->resize(ws.chars.width, ws.chars.height);
        screen_->render();
    }
}

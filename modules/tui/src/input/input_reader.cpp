#include <unistd.h>

#include <nx/core2/thread/thread.hpp>
#include <nx/tui/input/input_reader.hpp>

using namespace nx::tui;

input_reader::input_reader(nx::core::object * parent)
    : nx::core::object(parent)
{}

input_reader::~input_reader()
{
    stop();
}

void input_reader::start()
{
    if (!notifier_) {
        notifier_ = std::make_unique<nx::asio::handle_notifier>(
            nx::core::thread::current_context(),
            STDIN_FILENO,
            nx::asio::io_interest::read);
    }
    _arm();
}

void input_reader::stop()
{
    if (notifier_) {
        notifier_->cancel();
        notifier_.reset();
    }
    (void)parser_.flush(); // discard any incomplete sequence
}

void input_reader::_arm()
{
    if (!notifier_)
        return;

    notifier_->async_wait([this](nx::asio::native_handle_t /*fd*/,
                                 nx::asio::io_event        ev) {
        if (ev != nx::asio::io_event::none)
            _on_readable();
    });
}

void input_reader::_on_readable()
{
    uint8_t buf[64];
    const ssize_t n = ::read(STDIN_FILENO, buf, sizeof(buf));

    if (n > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            if (auto e = parser_.feed(buf[i]))
                _emit_event(*e);
        }
        // After draining the buffer, flush a pending lone ESC.
        if (auto e = parser_.flush())
            _emit_event(*e);
    }

    if (n >= 0)
        _arm();
    // n < 0 (error) or n == 0 (EOF): don't re-arm
}

void input_reader::_emit_event(const input_event & ev)
{
    if (const auto * ke = std::get_if<key_event>(&ev)) {
        NX_EMIT(key_pressed, *ke)
    } else if (const auto * me = std::get_if<mouse_event>(&ev)) {
        NX_EMIT(mouse_input, *me)
    }
}

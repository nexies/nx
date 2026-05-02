#pragma once

#include <memory>
#include <nx/common/platform.hpp>

#include <nx/core2/object/object.hpp>
#include <nx/asio/handle_notifier.hpp>

#include <nx/tui/input/escape_parser.hpp>
#include <nx/tui/types/size.hpp>

namespace nx::tui {

// ── input_reader ──────────────────────────────────────────────────────────────
//
// Asynchronously reads input and emits key / mouse / resize events.
//
// On POSIX:   watches stdin via a handle_notifier; raw bytes are decoded by
//             escape_parser (VT/SGR protocol).
//
// On Windows: watches the console input handle via a handle_notifier backed
//             by RegisterWaitForSingleObject in the IOCP backend; events are
//             translated directly from INPUT_RECORD (no escape parsing).
//
// Both platforms share the same start()/stop()/_arm() flow.
//
// Requires:
//   - Terminal must be in raw mode before start() is called.
//   - Must be started and stopped from the thread that owns the object.

class input_reader : public nx::core::object
{
public:
    NX_OBJECT(input_reader)

    explicit input_reader(nx::core::object * parent = nullptr);
    ~input_reader() override;

    void start();
    void stop();

    NX_SIGNAL(key_pressed,    key_event)
    NX_SIGNAL(mouse_input,    mouse_event)
    NX_SIGNAL(window_resized, window_size)

private:
    void _arm();
    void _on_readable();
    void _emit_event(const input_event & ev);

    std::unique_ptr<nx::asio::handle_notifier> notifier_;

#if defined(NX_OS_WINDOWS)
    unsigned long prev_button_state_ = 0; // DWORD: tracks button state for press/release diff
#else
    escape_parser parser_;
#endif
};

} // namespace nx::tui

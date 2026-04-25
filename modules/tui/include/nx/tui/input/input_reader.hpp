#pragma once

#include <memory>

#include <nx/core2/object/object.hpp>
#include <nx/asio/handle_notifier.hpp>

#include <nx/tui/input/escape_parser.hpp>

namespace nx::tui {

// ── input_reader ──────────────────────────────────────────────────────────────
//
// Asynchronously reads raw bytes from stdin and emits key / mouse events.
//
// Requires:
//   - Terminal must be in raw mode before start() is called.
//   - Must be started and stopped from the thread that owns the object
//     (the thread whose io_context will drive the async reads).
//
// Usage:
//   input_reader reader;
//   nx::core::connect(&reader, &input_reader::key_pressed,
//                     [](key_event e) { ... });
//   reader.start();

class input_reader : public nx::core::object
{
public:
    NX_OBJECT(input_reader)

    explicit input_reader(nx::core::object * parent = nullptr);
    ~input_reader() override;

    void start();
    void stop();

    NX_SIGNAL(key_pressed,  key_event)
    NX_SIGNAL(mouse_input,  mouse_event)

private:
    void _arm();
    void _on_readable();
    void _emit_event(const input_event & ev);

    std::unique_ptr<nx::asio::handle_notifier> notifier_;
    escape_parser                              parser_;
};

} // namespace nx::tui

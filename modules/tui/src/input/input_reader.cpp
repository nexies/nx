#include <nx/common/platform.hpp>

#if defined(NX_POSIX)
#  include <unistd.h>
#elif defined(NX_OS_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

#include <nx/core2/thread/thread.hpp>
#include <nx/tui/input/input_reader.hpp>
#include <nx/tui/terminal/terminal.hpp>

#if defined(NX_OS_WINDOWS)
namespace {

// Translate a Windows virtual key code to the corresponding VT escape sequence.
// Returns nullptr if the key has no VT equivalent (handled via uChar instead).
const char * vk_to_vt(WORD vk) noexcept
{
    switch (vk) {
    case VK_UP:     return "\x1b[A";
    case VK_DOWN:   return "\x1b[B";
    case VK_RIGHT:  return "\x1b[C";
    case VK_LEFT:   return "\x1b[D";
    case VK_HOME:   return "\x1b[H";
    case VK_END:    return "\x1b[F";
    case VK_INSERT: return "\x1b[2~";
    case VK_DELETE: return "\x1b[3~";
    case VK_PRIOR:  return "\x1b[5~";
    case VK_NEXT:   return "\x1b[6~";
    case VK_F1:     return "\x1bOP";
    case VK_F2:     return "\x1bOQ";
    case VK_F3:     return "\x1bOR";
    case VK_F4:     return "\x1bOS";
    case VK_F5:     return "\x1b[15~";
    case VK_F6:     return "\x1b[17~";
    case VK_F7:     return "\x1b[18~";
    case VK_F8:     return "\x1b[19~";
    case VK_F9:     return "\x1b[20~";
    case VK_F10:    return "\x1b[21~";
    case VK_F11:    return "\x1b[23~";
    case VK_F12:    return "\x1b[24~";
    default:        return nullptr;
    }
}

} // anonymous namespace
#endif

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
#if defined(NX_OS_WINDOWS)
        auto stdin_handle = static_cast<nx::asio::native_handle_t>(
            GetStdHandle(STD_INPUT_HANDLE));
#else
        auto stdin_handle = static_cast<nx::asio::native_handle_t>(STDIN_FILENO);
#endif
        notifier_ = std::make_unique<nx::asio::handle_notifier>(
            nx::core::thread::current_context(),
            stdin_handle,
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
#if defined(NX_OS_WINDOWS)
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD count = 0;
    if (!GetNumberOfConsoleInputEvents(hIn, &count) || count == 0) {
        _arm();
        return;
    }

    // Stack-allocate a reasonable batch; loop if the buffer has more.
    INPUT_RECORD records[64];
    DWORD        read_count = 0;
    if (!ReadConsoleInputW(hIn, records, 64, &read_count)) {
        _arm();
        return;
    }

    for (DWORD i = 0; i < read_count; ++i) {
        const INPUT_RECORD & rec = records[i];

        switch (rec.EventType) {
        case KEY_EVENT: {
            // Only handle key-down events to avoid duplicate emissions.
            if (!rec.Event.KeyEvent.bKeyDown)
                break;

            // First try a direct Unicode character (covers printable chars,
            // control chars, and keys with ENABLE_VIRTUAL_TERMINAL_INPUT).
            const wchar_t wch = rec.Event.KeyEvent.uChar.UnicodeChar;
            if (wch != 0) {
                // Convert from UTF-16 to UTF-8 and feed each byte to the parser.
                char utf8[4];
                const int len = WideCharToMultiByte(
                    CP_UTF8, 0, &wch, 1, utf8, sizeof(utf8), nullptr, nullptr);
                for (int j = 0; j < len; ++j)
                    if (auto e = parser_.feed(static_cast<uint8_t>(utf8[j])))
                        _emit_event(*e);
            } else {
                // No Unicode char — synthesize a VT sequence from the VK code.
                const char * seq = vk_to_vt(rec.Event.KeyEvent.wVirtualKeyCode);
                if (seq) {
                    for (const char * p = seq; *p; ++p)
                        if (auto e = parser_.feed(static_cast<uint8_t>(*p)))
                            _emit_event(*e);
                }
            }
            break;
        }

        case WINDOW_BUFFER_SIZE_EVENT: {
            auto ws = terminal::get_window_size();
            NX_EMIT(window_resized, ws)
            break;
        }

        // MOUSE_EVENT and FOCUS_EVENT are not handled yet.
        default:
            break;
        }
    }

    // Flush a pending lone ESC after draining the batch.
    if (auto e = parser_.flush())
        _emit_event(*e);

    _arm();

#else
    uint8_t buf[64];
    const int n = static_cast<int>(::read(STDIN_FILENO, buf, sizeof(buf)));

    if (n > 0) {
        for (int i = 0; i < n; ++i)
            if (auto e = parser_.feed(buf[i]))
                _emit_event(*e);
        if (auto e = parser_.flush())
            _emit_event(*e);
    }

    if (n >= 0)
        _arm();
    // n < 0 (error) or n == 0 (EOF): don't re-arm
#endif
}

void input_reader::_emit_event(const input_event & ev)
{
    if (const auto * ke = std::get_if<key_event>(&ev)) {
        NX_EMIT(key_pressed, *ke)
    } else if (const auto * me = std::get_if<mouse_event>(&ev)) {
        NX_EMIT(mouse_input, *me)
    }
}

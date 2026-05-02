#include <nx/common/platform.hpp>

#if defined(NX_POSIX)
#  include <unistd.h>
#  include <sys/select.h>
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

using namespace nx::tui;

// ── key translation ───────────────────────────────────────────────────────────

key vk_to_key(WORD vk) noexcept
{
    switch (vk) {
    case VK_UP:     return key::arrow_up;
    case VK_DOWN:   return key::arrow_down;
    case VK_LEFT:   return key::arrow_left;
    case VK_RIGHT:  return key::arrow_right;
    case VK_HOME:   return key::home;
    case VK_END:    return key::end;
    case VK_INSERT: return key::insert;
    case VK_DELETE: return key::delete_key;
    case VK_PRIOR:  return key::page_up;
    case VK_NEXT:   return key::page_down;
    case VK_F1:     return key::f1;
    case VK_F2:     return key::f2;
    case VK_F3:     return key::f3;
    case VK_F4:     return key::f4;
    case VK_F5:     return key::f5;
    case VK_F6:     return key::f6;
    case VK_F7:     return key::f7;
    case VK_F8:     return key::f8;
    case VK_F9:     return key::f9;
    case VK_F10:    return key::f10;
    case VK_F11:    return key::f11;
    case VK_F12:    return key::f12;
    case VK_RETURN: return key::enter;
    case VK_ESCAPE: return key::escape;
    case VK_BACK:   return key::backspace;
    case VK_TAB:    return key::tab;
    default:        return key::none;
    }
}

key_modifiers control_state_to_modifiers(DWORD cs) noexcept
{
    key_modifiers m;
    if (cs & SHIFT_PRESSED)                         m |= key_modifier::shift;
    if (cs & (LEFT_ALT_PRESSED  | RIGHT_ALT_PRESSED))  m |= key_modifier::alt;
    if (cs & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) m |= key_modifier::ctrl;
    return m;
}

std::optional<key_event> translate_key(const KEY_EVENT_RECORD & r) noexcept
{
    if (!r.bKeyDown)
        return std::nullopt;

    const key_modifiers mods = control_state_to_modifiers(r.dwControlKeyState);

    // Printable Unicode character — takes priority over VK.
    const wchar_t wch = r.uChar.UnicodeChar;
    if (wch >= 0x20 && wch != 0x7F) {
        // Ignore if this is just a modifier key generating a char (e.g. AltGr combos
        // that produce a printable char are fine; pure Ctrl+letter gives wch < 0x20).
        return key_event { key::printable, mods, static_cast<char32_t>(wch) };
    }

    // Control characters: map common ones explicitly.
    if (wch == '\r' || r.wVirtualKeyCode == VK_RETURN)  return key_event { key::enter,     mods };
    if (wch == 0x1B || r.wVirtualKeyCode == VK_ESCAPE)  return key_event { key::escape,    mods };
    if (wch == '\t' || r.wVirtualKeyCode == VK_TAB)     return key_event { key::tab,       mods };
    if (wch == 0x7F || r.wVirtualKeyCode == VK_BACK)    return key_event { key::backspace, mods };

    // For wch in [0x01..0x1A] (Ctrl+A..Ctrl+Z) emit as printable with ctrl mod.
    if (wch >= 0x01 && wch <= 0x1A) {
        const char32_t letter = L'a' + (wch - 1);
        return key_event { key::printable, mods | key_modifier::ctrl, letter };
    }

    // Navigation / function keys (wch == 0).
    const key k = vk_to_key(r.wVirtualKeyCode);
    if (k != key::none)
        return key_event { k, mods };

    return std::nullopt;
}

// ── mouse translation ─────────────────────────────────────────────────────────

std::optional<mouse_event> translate_mouse(const MOUSE_EVENT_RECORD & r,
                                           DWORD & prev_buttons) noexcept
{
    mouse_event me;

    // Coordinates: dwMousePosition is character-cell, 0-based.
    // mouse_event.position is col,row 1-based — add 1.
    me.position = { static_cast<int>(r.dwMousePosition.X) + 1,
                    static_cast<int>(r.dwMousePosition.Y) + 1 };

    me.modifiers = control_state_to_modifiers(r.dwControlKeyState);

    // ── Wheel ────────────────────────────────────────────────────────────────
    if (r.dwEventFlags & MOUSE_WHEELED) {
        const SHORT delta = static_cast<SHORT>(HIWORD(r.dwButtonState));
        me.action = mouse_action::wheel;
        me.button = (delta > 0) ? mouse_button::wheel_up : mouse_button::wheel_down;
        return me;
    }

    if (r.dwEventFlags & MOUSE_HWHEELED) {
        // Horizontal wheel — ignore for now.
        return std::nullopt;
    }

    // ── Move ─────────────────────────────────────────────────────────────────
    if (r.dwEventFlags & MOUSE_MOVED) {
        me.action = mouse_action::move;
        me.button = mouse_button::none;
        return me;
    }

    // ── Button press / release ────────────────────────────────────────────────
    // Diff the current button state against the previous one.
    const DWORD cur = r.dwButtonState & 0x1F; // lower 5 bits are the buttons
    const DWORD changed = cur ^ (prev_buttons & 0x1F);
    prev_buttons = r.dwButtonState;

    if (changed == 0) {
        // Windows sometimes fires a button event with no change (e.g. double-click
        // intermediate). Treat as a move to avoid phantom presses.
        me.action = mouse_action::move;
        me.button = mouse_button::none;
        return me;
    }

    // Find the first changed bit and report it.
    // FROM_LEFT_1ST_BUTTON_PRESSED = 0x0001  (left)
    // RIGHTMOST_BUTTON_PRESSED     = 0x0002  (right)
    // FROM_LEFT_2ND_BUTTON_PRESSED = 0x0004  (middle)
    if (changed & FROM_LEFT_1ST_BUTTON_PRESSED)
        me.button = mouse_button::left;
    else if (changed & RIGHTMOST_BUTTON_PRESSED)
        me.button = mouse_button::right;
    else if (changed & FROM_LEFT_2ND_BUTTON_PRESSED)
        me.button = mouse_button::middle;
    else
        return std::nullopt; // unsupported button

    me.action = (cur & changed) ? mouse_action::press : mouse_action::release;
    return me;
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
    if (notifier_)
        return; // already running

#if defined(NX_OS_WINDOWS)
    notifier_ = std::make_unique<nx::asio::handle_notifier>(
        nx::core::thread::current_context(),
        GetStdHandle(STD_INPUT_HANDLE),
        nx::asio::io_interest::read);
#else
    notifier_ = std::make_unique<nx::asio::handle_notifier>(
        nx::core::thread::current_context(),
        static_cast<nx::asio::native_handle_t>(STDIN_FILENO),
        nx::asio::io_interest::read);
#endif
    _arm();
}

void input_reader::stop()
{
    if (notifier_) {
        notifier_->cancel();
        notifier_.reset();
    }
#if !defined(NX_OS_WINDOWS)
    (void)parser_.flush(); // discard any incomplete sequence
#endif
}

void input_reader::_arm()
{
    if (!notifier_)
        return;

    notifier_->async_wait([this](nx::asio::native_handle_t /*handle*/,
                                 nx::asio::io_event        ev) {
        if (ev != nx::asio::io_event::none)
            _on_readable();
    });
}

void input_reader::_on_readable()
{
#if defined(NX_OS_WINDOWS)
    static HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD count = 0;
    if (!GetNumberOfConsoleInputEvents(hIn, &count) || count == 0)
        return;

    // Stack-allocate a reasonable batch; loop if the buffer has more.
    NX_THREAD_LOCAL INPUT_RECORD records[64];
    memset(records, 0, sizeof(INPUT_RECORD) * 64);
    DWORD        read_count = 0;
    if (!ReadConsoleInputW(hIn, records, 64, &read_count))
        return;

    for (DWORD i = 0; i < read_count; ++i) {
        const INPUT_RECORD & rec = records[i];

        switch (rec.EventType) {
        case KEY_EVENT: {
            if (auto ke = translate_key(rec.Event.KeyEvent))
                _emit_event(*ke);
            break;
        }

        case MOUSE_EVENT: {
            if (auto me = translate_mouse(rec.Event.MouseEvent, prev_button_state_))
                _emit_event(*me);
            break;
        }

        case WINDOW_BUFFER_SIZE_EVENT: {
            auto ws = terminal::get_window_size();
            NX_EMIT(window_resized, ws)
            break;
        }

        default:
            break;
        }
    }

    // Re-arm the one-shot NT thread-pool wait for the next event.
    _arm();

#else
    uint8_t buf[64];
    const int n = static_cast<int>(::read(STDIN_FILENO, buf, sizeof(buf)));

    if (n > 0) {
        for (int i = 0; i < n; ++i)
            if (auto e = parser_.feed(buf[i]))
                _emit_event(*e);

        // Only flush a pending lone ESC if no more bytes are immediately
        // available in the OS buffer.  When rapid events (e.g. mouse wheel)
        // arrive, an escape sequence can be split across two consecutive
        // read() calls — the first batch ends with the bare \x1b that opens
        // the next sequence.  Calling flush() in that case would incorrectly
        // emit a key::escape event.  select() with a zero timeout tells us
        // whether more bytes are already waiting; if so, _arm() will fire
        // _on_readable() again immediately and process them in context.
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        ::timeval tv = {0, 0};
        if (::select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) == 0) {
            if (auto e = parser_.flush())
                _emit_event(*e);
        }
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

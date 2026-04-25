#include <nx/tui/input/escape_parser.hpp>

using namespace nx::tui;

// ── Helpers ───────────────────────────────────────────────────────────────────

std::vector<int> escape_parser::_parse_params(std::string_view s)
{
    if (s.empty())
        return {};

    std::vector<int> result;
    int  n   = 0;
    bool has = false;

    for (char c : s) {
        if (c >= '0' && c <= '9') {
            n   = n * 10 + (c - '0');
            has = true;
        } else if (c == ';') {
            result.push_back(has ? n : 0);
            n   = 0;
            has = false;
        }
    }
    result.push_back(has ? n : 0);
    return result;
}

// xterm modifier parameter: mod - 1 encodes shift/alt/ctrl/meta in bits 0-3.
key_modifiers escape_parser::_xterm_mod(int code)
{
    const int m = code - 1;
    key_modifiers result;
    if (m & 0x01) result |= key_modifier::shift;
    if (m & 0x02) result |= key_modifier::alt;
    if (m & 0x04) result |= key_modifier::ctrl;
    if (m & 0x08) result |= key_modifier::meta;
    return result;
}

// ── Public API ────────────────────────────────────────────────────────────────

std::optional<input_event> escape_parser::feed(uint8_t byte)
{
    switch (state_) {
    case state::ground:
        if (byte == 0x1B) {
            state_ = state::escape;
            return std::nullopt;
        }
        switch (byte) {
        case 0x09: return key_event{ key::tab };
        case 0x0D: return key_event{ key::enter };
        case 0x0A: return key_event{ key::enter };
        case 0x7F: return key_event{ key::backspace };
        case 0x08: return key_event{ key::backspace };
        default:
            if (byte >= 0x01 && byte <= 0x1A) {
                // Ctrl+A..Ctrl+Z
                return key_event{ key::printable, key_modifier::ctrl,
                                  char32_t('@' + byte) };
            }
            if (byte >= 0x20 && byte <= 0x7E) {
                return key_event{ key::printable, {}, char32_t(byte) };
            }
            // UTF-8 lead byte: start multi-byte accumulation.
            if (byte >= 0xC0) {
                if (byte >= 0xF0) {
                    utf8_cp_ = byte & 0x07; utf8_remain_ = 3;
                } else if (byte >= 0xE0) {
                    utf8_cp_ = byte & 0x0F; utf8_remain_ = 2;
                } else {
                    utf8_cp_ = byte & 0x1F; utf8_remain_ = 1;
                }
                state_ = state::utf8;
                return std::nullopt;
            }
            // 0x80–0xBF: stray continuation byte — discard.
            return std::nullopt;
        }

    case state::escape:
        return _decode_escape(byte);

    case state::csi:
        if (byte >= 0x40 && byte <= 0x7E) {
            // Final byte — complete the sequence.
            auto ev  = _decode_csi(char(byte));
            state_   = state::ground;
            csi_priv_ = 0;
            csi_buf_.clear();
            return ev;
        }
        if (byte >= 0x30 && byte <= 0x3F) {
            // Parameter byte.
            // First byte: check for private parameter marker (<, =, >, ?).
            if (csi_priv_ == 0 && byte >= 0x3C) {
                csi_priv_ = char(byte);
            } else {
                csi_buf_ += char(byte);
            }
        }
        // Intermediate bytes 0x20..0x2F — silently ignored.
        return std::nullopt;

    case state::ss3:
        state_ = state::ground;
        return _decode_ss3(byte);

    case state::utf8:
        if ((byte & 0xC0) == 0x80) {
            // Valid continuation byte.
            utf8_cp_ = (utf8_cp_ << 6) | (byte & 0x3F);
            if (--utf8_remain_ == 0) {
                const auto cp = char32_t(utf8_cp_);
                utf8_cp_ = 0;
                state_   = state::ground;
                return key_event{ key::printable, {}, cp };
            }
        } else {
            // Invalid continuation — reset and reprocess this byte from ground.
            utf8_cp_  = 0;
            state_    = state::ground;
            return feed(byte);
        }
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<input_event> escape_parser::flush()
{
    if (state_ == state::escape) {
        state_ = state::ground;
        return key_event{ key::escape };
    }
    // Discard any incomplete CSI / SS3 / UTF-8 sequence.
    state_       = state::ground;
    csi_priv_    = 0;
    utf8_cp_     = 0;
    utf8_remain_ = 0;
    csi_buf_.clear();
    return std::nullopt;
}

// ── Escape sequences ──────────────────────────────────────────────────────────

std::optional<input_event> escape_parser::_decode_escape(uint8_t byte)
{
    switch (byte) {
    case 0x5B: // '['
        state_    = state::csi;
        csi_priv_ = 0;
        csi_buf_.clear();
        return std::nullopt;
    case 0x4F: // 'O'
        state_ = state::ss3;
        return std::nullopt;
    case 0x1B:
        // Double ESC: emit the first one, stay in escape state.
        return key_event{ key::escape };
    default:
        state_ = state::ground;
        if (byte >= 0x20 && byte <= 0x7E)
            return key_event{ key::printable, key_modifier::alt, char32_t(byte) };
        if (byte >= 0x01 && byte <= 0x1A)
            return key_event{ key::printable,
                              key_modifier::alt | key_modifier::ctrl,
                              char32_t('@' + byte) };
        return key_event{ key::escape };
    }
}

std::optional<input_event> escape_parser::_decode_ss3(uint8_t byte)
{
    switch (byte) {
    case 'P': return key_event{ key::f1 };
    case 'Q': return key_event{ key::f2 };
    case 'R': return key_event{ key::f3 };
    case 'S': return key_event{ key::f4 };
    case 'A': return key_event{ key::arrow_up };
    case 'B': return key_event{ key::arrow_down };
    case 'C': return key_event{ key::arrow_right };
    case 'D': return key_event{ key::arrow_left };
    case 'H': return key_event{ key::home };
    case 'F': return key_event{ key::end };
    default:  return std::nullopt;
    }
}

// ── CSI sequences ─────────────────────────────────────────────────────────────

std::optional<input_event> escape_parser::_decode_csi(char final_byte)
{
    if (csi_priv_ == '<')
        return _decode_mouse_sgr(final_byte);

    auto params = _parse_params(csi_buf_);
    auto get    = [&](int i, int def = 0) -> int {
        return (i < static_cast<int>(params.size())) ? params[i] : def;
    };

    // Modifier from second param (xterm-style: ESC [ 1 ; <mod> letter).
    const key_modifiers mods = (params.size() >= 2 && params[1] >= 2)
                               ? _xterm_mod(params[1])
                               : key_modifiers{};

    switch (final_byte) {
    case 'A': return key_event{ key::arrow_up,    mods };
    case 'B': return key_event{ key::arrow_down,  mods };
    case 'C': return key_event{ key::arrow_right, mods };
    case 'D': return key_event{ key::arrow_left,  mods };
    case 'H': return key_event{ key::home,        mods };
    case 'F': return key_event{ key::end,         mods };
    case 'P': return key_event{ key::f1 };
    case 'Q': return key_event{ key::f2 };
    case 'S': return key_event{ key::f4 };
    case 'R': return std::nullopt; // cursor position report — ignore
    case 'Z': return key_event{ key::tab, key_modifier::shift }; // Shift+Tab (backtab)
    case '~': {
        const key_modifiers mod2 = (params.size() >= 2 && params[1] >= 2)
                                   ? _xterm_mod(params[1])
                                   : key_modifiers{};
        switch (get(0)) {
        case 1:  return key_event{ key::home,      mod2 };
        case 2:  return key_event{ key::insert,    mod2 };
        case 3:  return key_event{ key::delete_key, mod2 };
        case 4:  return key_event{ key::end,       mod2 };
        case 5:  return key_event{ key::page_up,   mod2 };
        case 6:  return key_event{ key::page_down, mod2 };
        case 7:  return key_event{ key::home,      mod2 };
        case 8:  return key_event{ key::end,       mod2 };
        case 11: return key_event{ key::f1,        mod2 };
        case 12: return key_event{ key::f2,        mod2 };
        case 13: return key_event{ key::f3,        mod2 };
        case 14: return key_event{ key::f4,        mod2 };
        case 15: return key_event{ key::f5,        mod2 };
        case 17: return key_event{ key::f6,        mod2 };
        case 18: return key_event{ key::f7,        mod2 };
        case 19: return key_event{ key::f8,        mod2 };
        case 20: return key_event{ key::f9,        mod2 };
        case 21: return key_event{ key::f10,       mod2 };
        case 23: return key_event{ key::f11,       mod2 };
        case 24: return key_event{ key::f12,       mod2 };
        default: return std::nullopt;
        }
    }
    default:
        return std::nullopt;
    }
}

// ── SGR mouse ─────────────────────────────────────────────────────────────────
//
// Protocol: ESC [ < Pb ; Px ; Py M  (press / motion)
//                               m  (release)
//
// Pb bit layout:
//   bits 0-1 : button index (0=left, 1=middle, 2=right, 3=no-button/motion)
//   bit  2   : shift
//   bit  3   : alt / meta
//   bit  4   : ctrl
//   bit  5   : motion event (drag or hover)
//   bit  6   : wheel event  (bit 0 → 0=up, 1=down)

std::optional<input_event> escape_parser::_decode_mouse_sgr(char final_byte)
{
    auto params = _parse_params(csi_buf_);
    if (params.size() < 3)
        return std::nullopt;

    const int pb = params[0];
    const int px = params[1];
    const int py = params[2];

    key_modifiers mods;
    if (pb & 0x04) mods |= key_modifier::shift;
    if (pb & 0x08) mods |= key_modifier::alt;
    if (pb & 0x10) mods |= key_modifier::ctrl;

    mouse_event ev;
    ev.position  = { px, py };
    ev.modifiers = mods;

    if (pb & 0x40) {
        // Wheel
        ev.button = (pb & 0x01) ? mouse_button::wheel_down : mouse_button::wheel_up;
        ev.action = mouse_action::press;
    } else {
        switch (pb & 0x03) {
        case 0:  ev.button = mouse_button::left;   break;
        case 1:  ev.button = mouse_button::middle; break;
        case 2:  ev.button = mouse_button::right;  break;
        default: ev.button = mouse_button::none;   break;
        }

        if (final_byte == 'm') {
            ev.action = mouse_action::release;
        } else if (pb & 0x20) {
            ev.action = mouse_action::move;
        } else {
            ev.action = mouse_action::press;
        }
    }

    return ev;
}

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nx/tui/input/input_event.hpp>

namespace nx::tui {

// ── escape_parser ─────────────────────────────────────────────────────────────
//
// State machine that converts raw terminal bytes to input_event values.
//
// Usage:
//   escape_parser parser;
//   for (uint8_t byte : raw_bytes) {
//       if (auto ev = parser.feed(byte))
//           handle(*ev);
//   }
//   // After draining a read buffer, flush any pending lone ESC:
//   if (auto ev = parser.flush())
//       handle(*ev);
//
// Notes:
//   - UTF-8 multi-byte characters are not yet decoded; each continuation byte
//     is passed through as a separate key_event{key::printable}.
//   - Unrecognised escape sequences are silently discarded.

class escape_parser
{
public:
    escape_parser() = default;

    // Feed one byte. Returns an event if the byte completes a sequence.
    [[nodiscard]] std::optional<input_event> feed(uint8_t byte);

    // Flush a pending lone ESC (call after draining a read buffer).
    [[nodiscard]] std::optional<input_event> flush();

private:
    enum class state { ground, escape, csi, ss3, utf8 };

    state       state_       = state::ground;
    char        csi_priv_    = 0;   // '<', '?', '>', '=' or 0
    std::string csi_buf_;           // parameter digits and ';' accumulated in CSI

    // UTF-8 multi-byte accumulator
    uint32_t    utf8_cp_     = 0;   // codepoint being assembled
    int         utf8_remain_ = 0;   // continuation bytes still expected

    std::optional<input_event> _decode_escape(uint8_t byte);
    std::optional<input_event> _decode_csi(char final_byte);
    std::optional<input_event> _decode_ss3(uint8_t byte);
    std::optional<input_event> _decode_mouse_sgr(char final_byte);

    static key_modifiers      _xterm_mod(int code);
    static std::vector<int>   _parse_params(std::string_view s);
};

} // namespace nx::tui

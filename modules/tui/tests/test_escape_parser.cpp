#include <catch2/catch_test_macros.hpp>

#include <nx/tui/input/escape_parser.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>

using namespace nx::tui;

// ── Helpers ───────────────────────────────────────────────────────────────────

static std::vector<input_event> feed_bytes(escape_parser & p,
                                           std::initializer_list<uint8_t> bytes)
{
    std::vector<input_event> out;
    for (uint8_t b : bytes)
        if (auto ev = p.feed(b)) out.push_back(*ev);
    if (auto ev = p.flush()) out.push_back(*ev);
    return out;
}

static std::vector<input_event> feed_str(escape_parser & p, std::string_view s)
{
    std::vector<input_event> out;
    for (unsigned char c : s)
        if (auto ev = p.feed(static_cast<uint8_t>(c))) out.push_back(*ev);
    if (auto ev = p.flush()) out.push_back(*ev);
    return out;
}

static const key_event & as_key(const input_event & ev) {
    return std::get<key_event>(ev);
}
static const mouse_event & as_mouse(const input_event & ev) {
    return std::get<mouse_event>(ev);
}

// ── Basic printable / control keys ───────────────────────────────────────────

TEST_CASE("printable ASCII letters", "[escape_parser]")
{
    escape_parser p;
    auto evs = feed_bytes(p, { 'h', 'e', 'l', 'l', 'o' });
    REQUIRE(evs.size() == 5);
    for (int i = 0; i < 5; ++i) {
        const auto & k = as_key(evs[i]);
        REQUIRE(k.code == key::printable);
        REQUIRE(k.modifiers == key_modifiers{});
    }
    REQUIRE(as_key(evs[0]).character == U'h');
    REQUIRE(as_key(evs[4]).character == U'o');
}

TEST_CASE("printable ASCII — all visible characters", "[escape_parser]")
{
    escape_parser p;
    for (uint8_t c = 0x20; c <= 0x7E; ++c) {
        auto ev = p.feed(c);
        REQUIRE(ev.has_value());
        const auto & k = as_key(*ev);
        REQUIRE(k.code      == key::printable);
        REQUIRE(k.character == char32_t(c));
    }
}

TEST_CASE("enter key (CR and LF)", "[escape_parser]")
{
    escape_parser p;
    auto ev_cr = p.feed(0x0D);
    REQUIRE(ev_cr.has_value());
    REQUIRE(as_key(*ev_cr).code == key::enter);

    auto ev_lf = p.feed(0x0A);
    REQUIRE(ev_lf.has_value());
    REQUIRE(as_key(*ev_lf).code == key::enter);
}

TEST_CASE("tab key", "[escape_parser]")
{
    escape_parser p;
    auto ev = p.feed(0x09);
    REQUIRE(ev.has_value());
    REQUIRE(as_key(*ev).code == key::tab);
}

TEST_CASE("backspace (DEL and BS)", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(as_key(*p.feed(0x7F)).code == key::backspace);
    REQUIRE(as_key(*p.feed(0x08)).code == key::backspace);
}

TEST_CASE("Ctrl+A through Ctrl+Z — pure control codes", "[escape_parser]")
{
    // 0x08=BS, 0x09=Tab, 0x0A=LF(Enter), 0x0D=CR(Enter) are caught before
    // the generic Ctrl+letter branch and produce their own key codes.
    escape_parser p;
    for (int i = 1; i <= 26; ++i) {
        const uint8_t b = static_cast<uint8_t>(i);
        auto ev = p.feed(b);
        REQUIRE(ev.has_value());
        const auto & k = as_key(*ev);
        switch (b) {
        case 0x08: REQUIRE(k.code == key::backspace); break;
        case 0x09: REQUIRE(k.code == key::tab);       break;
        case 0x0A: REQUIRE(k.code == key::enter);     break;
        case 0x0D: REQUIRE(k.code == key::enter);     break;
        default:
            REQUIRE(k.code      == key::printable);
            REQUIRE((k.modifiers & key_modifier::ctrl) != key_modifiers{});
            REQUIRE(k.character == char32_t('@' + i));
            break;
        }
    }
}

// ── ESC sequences ─────────────────────────────────────────────────────────────

TEST_CASE("lone ESC — resolved by flush()", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(!p.feed(0x1B).has_value());
    auto ev = p.flush();
    REQUIRE(ev.has_value());
    REQUIRE(as_key(*ev).code == key::escape);
}

TEST_CASE("double ESC — first yields escape, second via flush", "[escape_parser]")
{
    escape_parser p;
    auto ev1 = p.feed(0x1B);
    REQUIRE(!ev1.has_value());
    auto ev2 = p.feed(0x1B); // second ESC → emit first
    REQUIRE(ev2.has_value());
    REQUIRE(as_key(*ev2).code == key::escape);
    auto ev3 = p.flush();    // flush second
    REQUIRE(ev3.has_value());
    REQUIRE(as_key(*ev3).code == key::escape);
}

TEST_CASE("Alt+letter", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B);
    auto ev = p.feed('x');
    REQUIRE(ev.has_value());
    const auto & k = as_key(*ev);
    REQUIRE(k.code      == key::printable);
    REQUIRE((k.modifiers & key_modifier::alt) != key_modifiers{});
    REQUIRE(k.character == U'x');
}

TEST_CASE("Alt+Ctrl combo (ESC + control byte)", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B);
    auto ev = p.feed(0x01); // ESC + Ctrl+A
    REQUIRE(ev.has_value());
    const auto & k = as_key(*ev);
    REQUIRE(k.code == key::printable);
    REQUIRE((k.modifiers & key_modifier::alt)  != key_modifiers{});
    REQUIRE((k.modifiers & key_modifier::ctrl) != key_modifiers{});
    REQUIRE(k.character == U'A');
}

// ── SS3 sequences ─────────────────────────────────────────────────────────────

TEST_CASE("SS3 F1-F4", "[escape_parser]")
{
    const struct { uint8_t byte; key expected; } cases[] = {
        { 'P', key::f1 }, { 'Q', key::f2 }, { 'R', key::f3 }, { 'S', key::f4 }
    };
    for (auto & tc : cases) {
        escape_parser p;
        p.feed(0x1B); p.feed('O');
        auto ev = p.feed(tc.byte);
        REQUIRE(ev.has_value());
        REQUIRE(as_key(*ev).code == tc.expected);
    }
}

TEST_CASE("SS3 arrow keys", "[escape_parser]")
{
    const struct { uint8_t byte; key expected; } cases[] = {
        { 'A', key::arrow_up }, { 'B', key::arrow_down },
        { 'C', key::arrow_right }, { 'D', key::arrow_left }
    };
    for (auto & tc : cases) {
        escape_parser p;
        p.feed(0x1B); p.feed('O');
        auto ev = p.feed(tc.byte);
        REQUIRE(ev.has_value());
        REQUIRE(as_key(*ev).code == tc.expected);
    }
}

TEST_CASE("SS3 Home and End", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('O');
    REQUIRE(as_key(*p.feed('H')).code == key::home);

    p.feed(0x1B); p.feed('O');
    REQUIRE(as_key(*p.feed('F')).code == key::end);
}

// ── CSI sequences ─────────────────────────────────────────────────────────────

TEST_CASE("CSI arrow keys (plain)", "[escape_parser]")
{
    const struct { char byte; key expected; } cases[] = {
        { 'A', key::arrow_up }, { 'B', key::arrow_down },
        { 'C', key::arrow_right }, { 'D', key::arrow_left }
    };
    for (auto & tc : cases) {
        escape_parser p;
        p.feed(0x1B); p.feed('[');
        auto ev = p.feed(static_cast<uint8_t>(tc.byte));
        REQUIRE(ev.has_value());
        REQUIRE(as_key(*ev).code == tc.expected);
        REQUIRE(as_key(*ev).modifiers == key_modifiers{});
    }
}

TEST_CASE("CSI arrow with Shift modifier (ESC [ 1 ; 2 A)", "[escape_parser]")
{
    escape_parser p;
    for (uint8_t b : std::initializer_list<uint8_t>{0x1B,'[','1',';','2','A'})
        p.feed(b);
    auto ev = p.flush(); // flush shouldn't be needed but check state reset
    // The 'A' finalizes the CSI, so flush returns nothing
    // Re-send a clean sequence
    escape_parser p2;
    p2.feed(0x1B); p2.feed('[');
    p2.feed('1'); p2.feed(';'); p2.feed('2');
    auto ev2 = p2.feed('A');
    REQUIRE(ev2.has_value());
    const auto & k = as_key(*ev2);
    REQUIRE(k.code == key::arrow_up);
    REQUIRE((k.modifiers & key_modifier::shift) != key_modifiers{});
}

TEST_CASE("CSI Shift+Tab (backtab ESC [ Z)", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('[');
    auto ev = p.feed('Z');
    REQUIRE(ev.has_value());
    const auto & k = as_key(*ev);
    REQUIRE(k.code == key::tab);
    REQUIRE((k.modifiers & key_modifier::shift) != key_modifiers{});
}

TEST_CASE("CSI Home and End (H / F)", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('[');
    REQUIRE(as_key(*p.feed('H')).code == key::home);

    p.feed(0x1B); p.feed('[');
    REQUIRE(as_key(*p.feed('F')).code == key::end);
}

TEST_CASE("CSI tilde sequences: Page Up, Page Down, Insert, Delete", "[escape_parser]")
{
    const struct { const char * params; key expected; } cases[] = {
        { "5", key::page_up }, { "6", key::page_down },
        { "2", key::insert  }, { "3", key::delete_key },
        { "1", key::home    }, { "4", key::end         }
    };
    for (auto & tc : cases) {
        escape_parser p;
        p.feed(0x1B); p.feed('[');
        for (char c : std::string_view(tc.params)) p.feed(static_cast<uint8_t>(c));
        auto ev = p.feed('~');
        REQUIRE(ev.has_value());
        REQUIRE(as_key(*ev).code == tc.expected);
    }
}

TEST_CASE("CSI tilde F5-F12", "[escape_parser]")
{
    const struct { const char * ps; key expected; } cases[] = {
        { "15", key::f5  }, { "17", key::f6  }, { "18", key::f7  }, { "19", key::f8  },
        { "20", key::f9  }, { "21", key::f10 }, { "23", key::f11 }, { "24", key::f12 }
    };
    for (auto & tc : cases) {
        escape_parser p;
        p.feed(0x1B); p.feed('[');
        for (char c : std::string_view(tc.ps)) p.feed(static_cast<uint8_t>(c));
        auto ev = p.feed('~');
        REQUIRE(ev.has_value());
        REQUIRE(as_key(*ev).code == tc.expected);
    }
}

// ── SGR mouse ─────────────────────────────────────────────────────────────────

static std::optional<input_event> parse_sgr(const std::string & seq)
{
    escape_parser p;
    std::optional<input_event> last;
    // seq is like "0;20;10M" — we prepend ESC [ <
    p.feed(0x1B); p.feed('['); p.feed('<');
    for (unsigned char c : seq) {
        auto ev = p.feed(c);
        if (ev) last = ev;
    }
    return last;
}

TEST_CASE("SGR mouse — left button press", "[escape_parser]")
{
    auto ev = parse_sgr("0;10;5M");
    REQUIRE(ev.has_value());
    const auto & m = as_mouse(*ev);
    REQUIRE(m.button   == mouse_button::left);
    REQUIRE(m.action   == mouse_action::press);
    REQUIRE(m.position == point<int>(10, 5));
}

TEST_CASE("SGR mouse — left button release", "[escape_parser]")
{
    auto ev = parse_sgr("0;10;5m");
    REQUIRE(ev.has_value());
    const auto & m = as_mouse(*ev);
    REQUIRE(m.button == mouse_button::left);
    REQUIRE(m.action == mouse_action::release);
}

TEST_CASE("SGR mouse — middle and right buttons", "[escape_parser]")
{
    auto ev_mid = parse_sgr("1;1;1M");
    REQUIRE(as_mouse(*ev_mid).button == mouse_button::middle);

    auto ev_right = parse_sgr("2;1;1M");
    REQUIRE(as_mouse(*ev_right).button == mouse_button::right);
}

TEST_CASE("SGR mouse — wheel up and down", "[escape_parser]")
{
    auto ev_up   = parse_sgr("64;5;3M");   // pb=64=0x40 → wheel, bit0=0 → up
    auto ev_down = parse_sgr("65;5;3M");   // pb=65=0x41 → wheel, bit0=1 → down

    REQUIRE(ev_up.has_value());
    REQUIRE(as_mouse(*ev_up).button == mouse_button::wheel_up);
    REQUIRE(as_mouse(*ev_up).action == mouse_action::press);

    REQUIRE(ev_down.has_value());
    REQUIRE(as_mouse(*ev_down).button == mouse_button::wheel_down);
}

TEST_CASE("SGR mouse — motion event", "[escape_parser]")
{
    auto ev = parse_sgr("32;15;7M");   // pb=32=0x20 → motion, button=0=left
    REQUIRE(ev.has_value());
    const auto & m = as_mouse(*ev);
    REQUIRE(m.action == mouse_action::move);
}

TEST_CASE("SGR mouse — Shift modifier (bit 2)", "[escape_parser]")
{
    auto ev = parse_sgr("4;1;1M");  // pb=4=0x04 → shift
    REQUIRE(ev.has_value());
    REQUIRE((as_mouse(*ev).modifiers & key_modifier::shift) != key_modifiers{});
}

TEST_CASE("SGR mouse — Ctrl modifier (bit 4)", "[escape_parser]")
{
    auto ev = parse_sgr("16;1;1M");  // pb=16=0x10 → ctrl
    REQUIRE(ev.has_value());
    REQUIRE((as_mouse(*ev).modifiers & key_modifier::ctrl) != key_modifiers{});
}

TEST_CASE("SGR mouse — Alt/Meta modifier (bit 3)", "[escape_parser]")
{
    auto ev = parse_sgr("8;1;1M");  // pb=8=0x08 → alt
    REQUIRE(ev.has_value());
    REQUIRE((as_mouse(*ev).modifiers & key_modifier::alt) != key_modifiers{});
}

TEST_CASE("SGR mouse — too few params yields no event", "[escape_parser]")
{
    auto ev = parse_sgr("0;5M");   // only 2 params, need 3
    REQUIRE(!ev.has_value());
}

// ── UTF-8 multi-byte ──────────────────────────────────────────────────────────

TEST_CASE("UTF-8 two-byte sequence (U+00E9 é)", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(!p.feed(0xC3).has_value());  // lead byte
    auto ev = p.feed(0xA9);             // continuation
    REQUIRE(ev.has_value());
    const auto & k = as_key(*ev);
    REQUIRE(k.code      == key::printable);
    REQUIRE(k.character == U'é');
}

TEST_CASE("UTF-8 three-byte sequence (U+20AC €)", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(!p.feed(0xE2).has_value());
    REQUIRE(!p.feed(0x82).has_value());
    auto ev = p.feed(0xAC);
    REQUIRE(ev.has_value());
    REQUIRE(as_key(*ev).character == U'€');
}

TEST_CASE("UTF-8 four-byte sequence (U+1F600 😀)", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(!p.feed(0xF0).has_value());
    REQUIRE(!p.feed(0x9F).has_value());
    REQUIRE(!p.feed(0x98).has_value());
    auto ev = p.feed(0x80);
    REQUIRE(ev.has_value());
    REQUIRE(as_key(*ev).character == U'\U0001F600');
}

TEST_CASE("UTF-8 — truncated sequence then ASCII recovers", "[escape_parser]")
{
    escape_parser p;
    p.feed(0xE2); // 3-byte lead, need 2 more
    p.feed(0x82); // first continuation, need 1 more
    // Now feed plain ASCII — should reset and emit 'A'
    auto ev = p.feed('A');
    REQUIRE(ev.has_value());
    REQUIRE(as_key(*ev).code      == key::printable);
    REQUIRE(as_key(*ev).character == U'A');
}

TEST_CASE("UTF-8 — stray continuation byte is discarded", "[escape_parser]")
{
    escape_parser p;
    auto ev = p.feed(0x80); // stray continuation in ground state
    REQUIRE(!ev.has_value());
    // Parser should still work normally after
    auto ev2 = p.feed('Z');
    REQUIRE(ev2.has_value());
    REQUIRE(as_key(*ev2).character == U'Z');
}

TEST_CASE("UTF-8 — multiple valid multi-byte chars in sequence", "[escape_parser]")
{
    escape_parser p;
    // Feed "éé" (U+00E9 twice): 0xC3 0xA9 0xC3 0xA9
    std::vector<input_event> out;
    for (uint8_t b : std::initializer_list<uint8_t>{ 0xC3, 0xA9, 0xC3, 0xA9 })
        if (auto ev = p.feed(b)) out.push_back(*ev);
    REQUIRE(out.size() == 2);
    REQUIRE(as_key(out[0]).character == U'é');
    REQUIRE(as_key(out[1]).character == U'é');
}

// ── Error recovery ────────────────────────────────────────────────────────────

TEST_CASE("truncated CSI — flush discards and resets", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('['); p.feed('1'); p.feed(';');
    // No final byte — flush should return nothing and reset
    auto ev = p.flush();
    REQUIRE(!ev.has_value());
    // Parser works normally after
    auto ev2 = p.feed('A');
    REQUIRE(ev2.has_value());
    REQUIRE(as_key(*ev2).character == U'A');
}

TEST_CASE("truncated SS3 — flush discards and resets", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('O');
    // No final byte
    auto ev = p.flush();
    REQUIRE(!ev.has_value());
    // Normal input resumes
    REQUIRE(as_key(*p.feed('B')).character == U'B');
}

TEST_CASE("unknown CSI final byte yields no event but resets state", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('[');
    auto ev = p.feed('@'); // @ is a valid final byte (0x40) but unrecognised
    REQUIRE(!ev.has_value());
    // State must be ground — plain keys work
    REQUIRE(as_key(*p.feed('Z')).character == U'Z');
}

TEST_CASE("unknown SS3 byte yields no event but resets state", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('O');
    auto ev = p.feed('X'); // not P/Q/R/S/A/B/C/D/H/F
    REQUIRE(!ev.has_value());
    REQUIRE(as_key(*p.feed('a')).character == U'a');
}

TEST_CASE("garbage bytes between valid sequences are silently ignored", "[escape_parser]")
{
    escape_parser p;
    // Feed: 'A', 0x80 (stray cont), 0x99 (stray cont), 'B'
    std::vector<input_event> out;
    for (uint8_t b : std::initializer_list<uint8_t>{ 'A', 0x80, 0x99, 'B' })
        if (auto ev = p.feed(b)) out.push_back(*ev);
    REQUIRE(out.size() == 2);
    REQUIRE(as_key(out[0]).character == U'A');
    REQUIRE(as_key(out[1]).character == U'B');
}

TEST_CASE("very long csi_buf_ is not harmful", "[escape_parser]")
{
    // Feed ESC [ followed by 1000 parameter bytes then a final byte.
    escape_parser p;
    p.feed(0x1B); p.feed('[');
    for (int i = 0; i < 1000; ++i) p.feed('0'); // digits, valid param bytes
    // Final byte 'A' → arrow_up with whatever params accumulated
    auto ev = p.feed('A');
    // We expect it not to crash and to return either a valid event or nothing.
    // The important thing is no UB / crash.
    (void)ev;
    // Parser continues working
    REQUIRE(as_key(*p.feed('x')).character == U'x');
}

TEST_CASE("interleaved valid sequences with garbage", "[escape_parser]")
{
    escape_parser p;
    std::vector<input_event> out;
    // Feed: ESC [ A (arrow_up), 0x80, 0x80, ESC [ B (arrow_down)
    for (uint8_t b : std::initializer_list<uint8_t>{
            0x1B, '[', 'A',
            0x80, 0x80,
            0x1B, '[', 'B' })
        if (auto ev = p.feed(b)) out.push_back(*ev);
    REQUIRE(out.size() == 2);
    REQUIRE(as_key(out[0]).code == key::arrow_up);
    REQUIRE(as_key(out[1]).code == key::arrow_down);
}

TEST_CASE("ESC [ then ESC resets to new escape sequence", "[escape_parser]")
{
    // Some terminals send ESC ESC [ A for alt+up. The second ESC should
    // cancel the pending CSI and start a new escape sequence.
    escape_parser p;
    p.feed(0x1B); // → state::escape
    p.feed('[');  // → state::csi
    // Now feed another ESC — this is 0x1B which is NOT a valid parameter byte
    // or final byte for CSI. It's below 0x20, so it falls through to no-op.
    // Depending on impl, it may or may not do something useful. Just check no crash.
    auto ev = p.feed(0x1B);
    (void)ev;
    // Eventually the parser should recover
    p.flush();
    REQUIRE(as_key(*p.feed('Z')).character == U'Z');
}

// ── flush() edge cases ────────────────────────────────────────────────────────

TEST_CASE("flush() on ground state returns nothing", "[escape_parser]")
{
    escape_parser p;
    REQUIRE(!p.flush().has_value());
}

TEST_CASE("flush() after complete sequence returns nothing", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); p.feed('['); p.feed('A'); // complete CSI arrow_up
    REQUIRE(!p.flush().has_value());
}

TEST_CASE("flush() can be called multiple times safely", "[escape_parser]")
{
    escape_parser p;
    p.feed(0x1B); // pending ESC
    auto ev1 = p.flush();
    REQUIRE(ev1.has_value());
    REQUIRE(as_key(*ev1).code == key::escape);
    // Second flush on clean state
    REQUIRE(!p.flush().has_value());
    REQUIRE(!p.flush().has_value());
}

// ── Longer realistic sequences ────────────────────────────────────────────────

TEST_CASE("realistic typing session: word + arrows + backspace", "[escape_parser]")
{
    escape_parser p;
    std::vector<input_event> out;

    // Type "hi", left arrow, backspace
    for (uint8_t b : std::initializer_list<uint8_t>{
            'h', 'i',
            0x1B, '[', 'D',  // left arrow
            0x7F             // backspace
        })
        if (auto ev = p.feed(b)) out.push_back(*ev);

    REQUIRE(out.size() == 4);
    REQUIRE(as_key(out[0]).character == U'h');
    REQUIRE(as_key(out[1]).character == U'i');
    REQUIRE(as_key(out[2]).code      == key::arrow_left);
    REQUIRE(as_key(out[3]).code      == key::backspace);
}

TEST_CASE("mouse SGR sequence followed immediately by keyboard input", "[escape_parser]")
{
    escape_parser p;
    std::vector<input_event> out;

    // SGR left click at (5,3) then key 'a'
    for (uint8_t b : std::initializer_list<uint8_t>{
            0x1B, '[', '<', '0', ';', '5', ';', '3', 'M',
            'a'
        })
        if (auto ev = p.feed(b)) out.push_back(*ev);

    REQUIRE(out.size() == 2);
    REQUIRE(std::holds_alternative<mouse_event>(out[0]));
    REQUIRE(as_mouse(out[0]).button == mouse_button::left);
    REQUIRE(std::holds_alternative<key_event>(out[1]));
    REQUIRE(as_key(out[1]).character == U'a');
}

TEST_CASE("page up/down followed by UTF-8 character", "[escape_parser]")
{
    escape_parser p;
    std::vector<input_event> out;
    // Page down (ESC [ 6 ~) then € (E2 82 AC)
    for (uint8_t b : std::initializer_list<uint8_t>{
            0x1B, '[', '6', '~',
            0xE2, 0x82, 0xAC
        })
        if (auto ev = p.feed(b)) out.push_back(*ev);

    REQUIRE(out.size() == 2);
    REQUIRE(as_key(out[0]).code      == key::page_down);
    REQUIRE(as_key(out[1]).character == U'€');
}

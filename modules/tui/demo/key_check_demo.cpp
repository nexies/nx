// key_check_demo — press every listed key / mouse action to check it off.
//
// Each entry is displayed as a small cell.  Unchecked → gray.  Checked → green.
// When all entries are checked the program exits automatically.
// Pressing Escape exits immediately (handled by tui_application).
//
// Modifier abbreviations used in labels:
//   C = Ctrl   A = Alt   S = Shift   M = Meta
//   Combined: "CA+Up", "CS+F12", …

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/event_filter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace nx::tui;

// ── compact naming helpers ────────────────────────────────────────────────────

static std::string mod_prefix(key_modifiers m)
{
    std::string s;
    if (m.has(key_modifier::ctrl))  s += 'C';
    if (m.has(key_modifier::alt))   s += 'A';
    if (m.has(key_modifier::shift)) s += 'S';
    if (m.has(key_modifier::meta))  s += 'M';
    if (!s.empty()) s += '+';
    return s;
}

static const char * key_short(key k)
{
    switch (k) {
    case key::enter:       return "Enter";
    case key::tab:         return "Tab";
    case key::backspace:   return "BS";
    case key::delete_key:  return "Del";
    case key::insert:      return "Ins";
    case key::home:        return "Home";
    case key::end:         return "End";
    case key::page_up:     return "PgUp";
    case key::page_down:   return "PgDn";
    case key::arrow_up:    return "Up";
    case key::arrow_down:  return "Dn";
    case key::arrow_left:  return "Lt";
    case key::arrow_right: return "Rt";
    case key::f1:  return "F1";  case key::f2:  return "F2";
    case key::f3:  return "F3";  case key::f4:  return "F4";
    case key::f5:  return "F5";  case key::f6:  return "F6";
    case key::f7:  return "F7";  case key::f8:  return "F8";
    case key::f9:  return "F9";  case key::f10: return "F10";
    case key::f11: return "F11"; case key::f12: return "F12";
    default: return "?";
    }
}

// ── entry descriptors ─────────────────────────────────────────────────────────

struct key_entry {
    std::string   label;
    key           code      = key::none;
    key_modifiers mods;
    char32_t      character = 0;   // non-zero → also match e.character
    bool          checked   = false;
};

struct mouse_entry {
    std::string   label;
    mouse_button  button = mouse_button::none;
    mouse_action  action = mouse_action::move;
    bool          checked = false;
};

// ── key_check_widget ──────────────────────────────────────────────────────────

class key_check_widget : public widget, public event_filter {
    std::vector<key_entry>   keys_;
    std::vector<mouse_entry> mice_;
    int                      total_   = 0;
    int                      checked_ = 0;

    // Last-activated tracking: whichever entry was matched most recently
    // (including re-presses of already-checked entries).
    enum class last_kind { none, key, mouse };
    last_kind last_kind_ = last_kind::none;
    int       last_idx_  = -1;

public:
    NX_OBJECT(key_check_widget)
    NX_SIGNAL(all_done)

    explicit key_check_widget(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::strong_focus);
        _build_entries();
        total_ = static_cast<int>(keys_.size() + mice_.size());
        // Install self as filter so Tab events are intercepted before the screen's
        // focus-cycling logic runs (which would swallow them otherwise).
        install_event_filter(this);
    }

private:
    // ── Entry builder helpers ─────────────────────────────────────────────────

    void _k(key k, key_modifiers m = {})
    {
        keys_.push_back({ mod_prefix(m) + key_short(k), k, m });
    }

    // Printable key with modifier (ctrl/alt + letter).
    void _p(char ch, key_modifiers m)
    {
        std::string label = mod_prefix(m);
        label += ch;
        keys_.push_back({ label, key::printable, m, char32_t(ch) });
    }

    void _m(const char * label, mouse_button b, mouse_action a)
    {
        mice_.push_back({ label, b, a });
    }

    void _build_entries()
    {
        // ── Arrow keys × {none, S, C, A, CS} ─────────────────────────────────
        for (auto k : { key::arrow_up, key::arrow_down,
                        key::arrow_left, key::arrow_right }) {
            _k(k);
            _k(k, key_modifier::shift);
            _k(k, key_modifier::ctrl);
            _k(k, key_modifier::alt);
            _k(k, key_modifier::ctrl | key_modifier::shift);
        }

        // ── Home / End / PgUp / PgDn × {none, S, C, CS} ──────────────────────
        for (auto k : { key::home, key::end,
                        key::page_up, key::page_down }) {
            _k(k);
            _k(k, key_modifier::shift);
            _k(k, key_modifier::ctrl);
            _k(k, key_modifier::ctrl | key_modifier::shift);
        }

        // ── Editing keys ──────────────────────────────────────────────────────
        _k(key::enter);
        _k(key::enter, key_modifier::ctrl);
        _k(key::enter, key_modifier::alt);

        _k(key::tab);
        _k(key::tab, key_modifier::shift);
        _k(key::tab, key_modifier::ctrl);
        _k(key::tab, key_modifier::alt);
        _k(key::tab, key_modifier::ctrl | key_modifier::shift);
        _k(key::tab, key_modifier::alt  | key_modifier::shift);

        _k(key::backspace);
        _k(key::backspace, key_modifier::ctrl);
        _k(key::backspace, key_modifier::alt);

        _k(key::delete_key);
        _k(key::delete_key, key_modifier::ctrl);
        _k(key::delete_key, key_modifier::shift);

        _k(key::insert);
        _k(key::insert, key_modifier::shift);
        _k(key::insert, key_modifier::ctrl);

        // ── Function keys × {none, S, C, A} ──────────────────────────────────
        for (auto k : { key::f1,  key::f2,  key::f3,  key::f4,
                        key::f5,  key::f6,  key::f7,  key::f8,
                        key::f9,  key::f10, key::f11, key::f12 }) {
            _k(k);
            _k(k, key_modifier::shift);
            _k(k, key_modifier::ctrl);
            _k(k, key_modifier::alt);
        }

        // ── Ctrl+printable samples ────────────────────────────────────────────
        // Parser encodes ctrl+A..Z as key::printable + ctrl modifier + 'A'..'Z'
        for (char c : { 'A', 'C', 'D', 'V', 'X', 'Z' })
            _p(c, key_modifier::ctrl);

        // ── Alt+printable samples ─────────────────────────────────────────────
        for (char c : { 'a', 'h', 'z' })
            _p(c, key_modifier::alt);

        // ── Mouse ─────────────────────────────────────────────────────────────
        _m("LClick",  mouse_button::left,       mouse_action::press);
        _m("LRel",    mouse_button::left,       mouse_action::release);
        _m("RClick",  mouse_button::right,      mouse_action::press);
        _m("RRel",    mouse_button::right,      mouse_action::release);
        _m("MClick",  mouse_button::middle,     mouse_action::press);
        _m("WheelUp", mouse_button::wheel_up,   mouse_action::press);
        _m("WheelDn", mouse_button::wheel_down, mouse_action::press);
        _m("Move",    mouse_button::none,       mouse_action::move);
    }

    // ── event_filter override ─────────────────────────────────────────────────
    // Intercept Tab (all modifier combos) before the screen's focus-cycling
    // logic consumes them.  Forward to on_key_press and return true (consumed).

    bool filter_key(key_event & e) override
    {
        if (e.code == key::tab) {
            on_key_press(e);
            return true;
        }
        return false;
    }

    // ── Paint ─────────────────────────────────────────────────────────────────

    void on_paint(painter & p) override
    {
        p.fill(" ");

        const int w = size().width;
        const int h = size().height;
        if (w <= 0 || h <= 0) return;

        constexpr int CELL = 10;   // chars per cell (label padded to CELL-1 + space)
        const int cols = std::max(1, w / CELL);

        // ── Header ────────────────────────────────────────────────────────────
        {
            painter hp = p;
            hp.set_color(color::cyan_bright);
            hp.draw_text({0, 0}, " Key/Input Checker");
            hp.set_color(color::gray_dark);
            hp.draw_text({19, 0}, "  Esc: quit");

            const std::string prog = std::to_string(checked_)
                                   + "/" + std::to_string(total_);
            hp.set_color(checked_ == total_ ? color::green_bright : color::white);
            hp.draw_text({w - static_cast<int>(prog.size()) - 1, 0}, prog);
        }

        // ── Grid drawing helpers ──────────────────────────────────────────────
        int row = 2;
        int col = 0;

        auto flush_to_new_row = [&]() {
            if (col != 0) { col = 0; ++row; }
        };

        auto draw_cell = [&](const std::string & label, bool checked, bool is_last) {
            if (row >= h) return;

            painter cp = p;
            if (is_last) {
                // Highlight the last-activated cell regardless of checked state.
                cp.set_color(checked ? color::green_bright : color::white);
                cp.set_background_color(color::blue);
            } else {
                cp.set_color(checked ? color::green_bright : color::gray_dark);
            }

            // Pad or truncate label to CELL-1 chars, then one trailing space.
            std::string cell = label;
            if (static_cast<int>(cell.size()) > CELL - 1)
                cell.resize(CELL - 1);
            else
                cell.resize(CELL - 1, ' ');
            cell += ' ';

            cp.draw_text({ col * CELL, row }, cell);

            if (++col >= cols) { col = 0; ++row; }
        };

        auto draw_section_header = [&](const char * title) {
            flush_to_new_row();
            if (row >= h) return;
            painter sp = p;
            sp.set_color(color::cyan);
            std::string line = title;
            line.resize(w, ' ');
            sp.draw_text({0, row}, line);
            ++row;
        };

        // ── Key entries ───────────────────────────────────────────────────────
        draw_section_header(" Keys: (C=Ctrl  A=Alt  S=Shift)");

        for (int i = 0; i < static_cast<int>(keys_.size()); ++i)
            draw_cell(keys_[i].label, keys_[i].checked,
                      last_kind_ == last_kind::key && last_idx_ == i);

        // ── Mouse entries ─────────────────────────────────────────────────────
        flush_to_new_row();
        ++row;   // blank line between sections
        draw_section_header(" Mouse:");

        for (int i = 0; i < static_cast<int>(mice_.size()); ++i)
            draw_cell(mice_[i].label, mice_[i].checked,
                      last_kind_ == last_kind::mouse && last_idx_ == i);

        // ── Done banner ───────────────────────────────────────────────────────
        if (checked_ == total_) {
            painter fp = p;
            fp.set_color(color::green_bright);
            fp.draw_text({0, h - 1}, " All done! Exiting…");
        }
    }

    // ── Key events ────────────────────────────────────────────────────────────

    void on_key_press(key_event & e) override
    {
        for (int i = 0; i < static_cast<int>(keys_.size()); ++i) {
            auto & ke = keys_[i];
            if (ke.code != e.code) continue;
            if (ke.mods != e.modifiers) continue;
            if (ke.code == key::printable && ke.character != e.character) continue;
            // Always update last-activated (even if already checked).
            last_kind_ = last_kind::key;
            last_idx_  = i;
            if (!ke.checked) {
                ke.checked = true;
                ++checked_;
            }
            break;
        }
        update();
        if (checked_ == total_) NX_EMIT(all_done)
    }

    // ── Mouse events ──────────────────────────────────────────────────────────

    void on_mouse_press  (mouse_event & e) override { _handle_mouse(e); }
    void on_mouse_release(mouse_event & e) override { _handle_mouse(e); }
    void on_mouse_move   (mouse_event & e) override { _handle_mouse(e); }
    void on_wheel        (mouse_event & e) override { _handle_mouse(e); }

    void _handle_mouse(mouse_event & e)
    {
        for (int i = 0; i < static_cast<int>(mice_.size()); ++i) {
            auto & me = mice_[i];
            if (me.button != e.button) continue;
            if (me.action != e.action) continue;
            // Always update last-activated (even if already checked).
            last_kind_ = last_kind::mouse;
            last_idx_  = i;
            if (!me.checked) {
                me.checked = true;
                ++checked_;
            }
            break;
        }
        update();
        if (checked_ == total_) NX_EMIT(all_done)
    }
};

// ── Controller ────────────────────────────────────────────────────────────────

class app_ctrl : public nx::core::object {
    tui_application & app_;
public:
    NX_OBJECT(app_ctrl)
    explicit app_ctrl(tui_application & app, nx::core::object * parent = nullptr)
        : nx::core::object(parent), app_(app) {}
    void on_done() { app_.quit(); }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);

    auto * scr = app.main_screen();
    scr->set_style(bg(color::black) | fg(color::white));

    auto * checker = new key_check_widget(scr);
    scr->set_focused_widget(checker);

    app_ctrl ctrl(app);
    nx::core::connect(checker, &key_check_widget::all_done,
                      &ctrl,   &app_ctrl::on_done);

    return app.exec();
}

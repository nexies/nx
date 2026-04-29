#pragma once

#include <nx/tui/widgets/widget.hpp>

#include <string>

namespace nx::tui {

// ── button ────────────────────────────────────────────────────────────────────
//
// Clickable single-row widget.
// Emits clicked() on left mouse press or Enter/Space when focused.
// Visually inverts fg/bg when focused (unless the widget's style overrides it).

class button : public widget {
    std::string text_;
    bool pressed_ = false;

public:
    NX_OBJECT(button)

    explicit button(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void
    set_text(std::string t);

    [[nodiscard]] const std::string &
    text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // Pressed
    NX_NODISCARD constexpr bool
    is_pressed() const noexcept { return pressed_; }

    NX_PROPERTY(pressed, MEMBER pressed_, READ is_pressed)


    // ── Signals ───────────────────────────────────────────────────────────────

    void click();

    NX_SIGNAL(clicked)

    [[nodiscard]] size_type size_hint() const override;



protected:
    void on_paint(painter & p)       override;
    void on_key_press(key_event & e) override;
    void on_mouse_press(mouse_event & e) override;
    void on_mouse_release(mouse_event& e) override;
};

} // namespace nx::tui

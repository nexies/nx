#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/orientation.hpp>

namespace nx::tui {

// ── slider ────────────────────────────────────────────────────────────────────
//
// A draggable value control on a linear track.
//
// Visual (horizontal, height = 1):
//   ━━━━━━━━━━━━●────────────────   filled=accent, empty=border, thumb=accent
//
// Visual (vertical, width = 1):
//   │  (empty above)
//   ●  (thumb)
//   ┃  (filled below)
//
// Keyboard: ←/→ (or ↑/↓) step, PgUp/PgDn page-step, Home/End min/max.
// Mouse:    click anywhere on the track to jump.

class slider : public widget {
    int         min_       = 0;
    int         max_       = 100;
    int         value_     = 0;
    int         step_      = 1;
    int         page_step_ = 10;
    orientation orient_    = orientation::horizontal;

public:
    NX_OBJECT(slider)

    explicit slider(nx::core::object * parent = nullptr);

    // ── Range & value ─────────────────────────────────────────────────────────

    void set_range(int min, int max);
    void set_value(int v);
    void set_step     (int s) noexcept { step_      = s; }
    void set_page_step(int s) noexcept { page_step_ = s; }
    void set_orientation(orientation o);

    [[nodiscard]] int         value()       const noexcept { return value_; }
    [[nodiscard]] int         minimum()     const noexcept { return min_;   }
    [[nodiscard]] int         maximum()     const noexcept { return max_;   }
    [[nodiscard]] int         step()        const noexcept { return step_;  }
    [[nodiscard]] int         page_step()   const noexcept { return page_step_; }
    [[nodiscard]] orientation orient()      const noexcept { return orient_; }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(value_changed, int)
    NX_SIGNAL(range_changed, int, int)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)           override;
    void on_key_press(key_event & e)     override;
    void on_mouse_press(mouse_event & e) override;
    void on_wheel(mouse_event & e)       override;
    void on_focus_in()                   override;
    void on_focus_out()                  override;
    [[nodiscard]] bool _intercepts_wheel() const noexcept override { return true; }

private:
    void _set_value_clamped(int v);
    [[nodiscard]] int _track_len()        const noexcept;
    [[nodiscard]] int _value_to_pos(int v) const noexcept;
    [[nodiscard]] int _pos_to_value(int p) const noexcept;
};

} // namespace nx::tui

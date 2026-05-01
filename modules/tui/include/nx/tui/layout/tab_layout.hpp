#pragma once

#include <nx/tui/widgets/widget.hpp>

#include <vector>

namespace nx::tui {

// ── tab_layout ────────────────────────────────────────────────────────────────
//
// Shows exactly one child at a time.  All children share the same rect
// (the full widget area).  Switching tabs hides the previous child and shows
// the new one.
//
// add_tab(w)       — reparent w to this layout and register it as a tab.
//                    The first added tab is automatically made current.
// set_current(idx) — switch to tab at index idx; emits current_changed.
// remove_tab(w)    — remove and reparent w to nullptr.

class tab_layout : public widget {
public:
    NX_OBJECT(tab_layout)

    explicit tab_layout(nx::core::object * parent = nullptr);

    void add_tab(widget * w);
    void remove_tab(widget * w);

    // Switch to tab at index idx (0-based).
    void set_current(int idx);
    [[nodiscard]] int current() const noexcept { return current_; }
    [[nodiscard]] int count()   const noexcept
    { return static_cast<int>(tabs_.size()); }

    // Emitted after the current tab changes.  Carries the new index.
    NX_SIGNAL(current_changed, int)

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;

private:
    std::vector<widget *> tabs_;
    int                   current_ = -1;
};

} // namespace nx::tui

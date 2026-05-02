#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/widgets/tab_bar.hpp>
#include <nx/tui/layout/tab_layout.hpp>

#include <string>

namespace nx::tui {

// ── tab_widget ────────────────────────────────────────────────────────────────
//
// Composite widget: tab_bar (1 row) + tab_layout (all remaining rows).
//
// Usage:
//   auto * tabs = new tab_widget(parent);
//   auto * page = new v_box(tabs);  // or any other widget
//   // … populate page …
//   tabs->add_tab(page, "Name");
//
// Tab switching is keyboard-accessible via the tab_bar (Tab/arrow keys).
// Programmatic switching: set_current(idx).

class tab_widget : public widget {
    tab_bar    * bar_;
    tab_layout * content_;

public:
    NX_OBJECT(tab_widget)

    explicit tab_widget(nx::core::object * parent = nullptr);

    // ── Tab management ────────────────────────────────────────────────────────

    void add_tab(widget * page, std::string title);
    void set_current(int idx);

    [[nodiscard]] int current() const noexcept;
    [[nodiscard]] int count()   const noexcept;

    [[nodiscard]] tab_bar    *       bar()       noexcept { return bar_;     }
    [[nodiscard]] tab_layout *       content()   noexcept { return content_; }
    [[nodiscard]] const tab_bar    * bar()  const noexcept { return bar_;     }
    [[nodiscard]] const tab_layout * content() const noexcept { return content_; }

    NX_SIGNAL(current_changed, int)

protected:
    void _apply_layout() override;
};

} // namespace nx::tui

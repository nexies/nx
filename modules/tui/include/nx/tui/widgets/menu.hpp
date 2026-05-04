#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/widgets/menu_item.hpp>

#include <vector>
#include <string>

namespace nx::tui {

class scroll_area;
class v_box;

// ── menu ──────────────────────────────────────────────────────────────────────
//
// Scrollable list of menu_item widgets with keyboard navigation and animated
// selection.
//
// Composition: menu owns a scroll_area that contains a v_box of menu_items.
// The menu itself takes focus; scroll_area and items do not.
//
// Usage — convenience factory:
//   auto * m = new menu(parent);
//   m->add_entry("Option A");
//   m->add_entry("Option B");
//   nx::core::connect(m, &menu::activated, this, [](int i) { ... });
//
// Usage — custom item subclass:
//   auto * item = new my_item(nullptr);  // parent=nullptr
//   item->set_icon(…);
//   m->append(item);                     // menu reparents it into its v_box
//
// Keyboard:
//   Up / Down       — move selection by one
//   Page Up / Down  — move selection by one viewport page
//   Home / End      — jump to first / last item
//   Enter / Space   — emit activated(current)

class menu : public widget {
    scroll_area *           sa_;
    v_box *                 box_;
    std::vector<menu_item*> items_;
    int                     current_ = -1;

public:
    NX_OBJECT(menu)

    explicit menu(nx::core::object * parent = nullptr);

    // ── Item management ───────────────────────────────────────────────────────

    // Reparents item (created with parent=nullptr) into the menu's v_box.
    menu_item * append(menu_item * item);

    // Creates a plain text menu_item and appends it.
    menu_item * add_entry(std::string text);

    // Removes and destroys the item at index.
    void remove(int index);

    // Removes and destroys all items.
    void clear();

    // ── Selection ─────────────────────────────────────────────────────────────

    [[nodiscard]] int         current()     const noexcept { return current_; }
    [[nodiscard]] int         count()       const noexcept;
    [[nodiscard]] menu_item * item_at(int i) const noexcept;

    void set_current(int index);

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(current_changed, int) // fires on every selection change
    NX_SIGNAL(activated, int)       // fires on Enter, Space, or click

    [[nodiscard]] size_type size_hint() const override;

protected:
    void _apply_layout()              override;
    void on_key_press(key_event & e)  override;

private:
    void _on_item_clicked(menu_item * item);
    void _ensure_visible(int index);
    menu_item * _register(menu_item * item);
};

} // namespace nx::tui

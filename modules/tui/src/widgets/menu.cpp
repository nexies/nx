#include <nx/tui/widgets/menu.hpp>
#include <nx/tui/widgets/scroll_area.hpp>
#include <nx/tui/layout/box.hpp>
#include <nx/tui/input/key_event.hpp>

#include <algorithm>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

menu::menu(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    sa_  = new scroll_area(this);
    box_ = new v_box(sa_);
    box_->set_spacing(0);
}

// ── Item management ───────────────────────────────────────────────────────────

menu_item * menu::append(menu_item * item)
{
    item->set_parent(box_);
    return _register(item);
}

menu_item * menu::add_entry(std::string text)
{
    auto * item = new menu_item(box_);
    item->set_text(std::move(text));
    return _register(item);
}

menu_item * menu::_register(menu_item * item)
{
    items_.push_back(item);
    // Capture item pointer so the index is looked up dynamically — remains
    // correct even if earlier items are removed after this connection is made.
    nx::core::connect(item, &menu_item::clicked, this,
        [this, item]() { _on_item_clicked(item); });
    update();
    return item;
}

void menu::remove(int index)
{
    if (index < 0 || index >= count()) return;

    delete items_[static_cast<std::size_t>(index)]; // widget destructor unparents + cleans connections
    items_.erase(items_.begin() + index);

    if (current_ == index)
        current_ = items_.empty() ? -1 : std::min(index, count() - 1);
    else if (current_ > index)
        --current_;

    // Re-apply selection highlight to the new current item.
    if (current_ >= 0)
        items_[static_cast<std::size_t>(current_)]->set_selected(true);

    update();
}

void menu::clear()
{
    for (auto * item : items_)
        delete item;
    items_.clear();
    current_ = -1;
    update();
}

// ── Accessors ─────────────────────────────────────────────────────────────────

int menu::count() const noexcept
{
    return static_cast<int>(items_.size());
}

menu_item * menu::item_at(int i) const noexcept
{
    if (i < 0 || i >= count()) return nullptr;
    return items_[static_cast<std::size_t>(i)];
}

// ── Selection ─────────────────────────────────────────────────────────────────

void menu::set_current(int index)
{
    if (index < -1 || index >= count()) return;
    if (index == current_) return;

    if (current_ >= 0 && current_ < count())
        items_[static_cast<std::size_t>(current_)]->set_selected(false);

    current_ = index;

    if (current_ >= 0)
        items_[static_cast<std::size_t>(current_)]->set_selected(true);

    _ensure_visible(current_);
    NX_EMIT(current_changed, current_)
}

void menu::_on_item_clicked(menu_item * item)
{
    const auto it = std::find(items_.begin(), items_.end(), item);
    if (it == items_.end()) return;
    const int index = static_cast<int>(it - items_.begin());
    set_current(index);
    NX_EMIT(activated, index)
}

void menu::_ensure_visible(int index)
{
    if (index < 0 || index >= count()) return;
    // Each item has height 1 with spacing 0, so item i sits at y=i.
    const int sy = sa_->scroll_y();
    const int vh = sa_->size().height;
    if (index < sy)
        sa_->scroll_to(0, index);
    else if (index >= sy + vh)
        sa_->scroll_to(0, index - vh + 1);
}

// ── Layout ────────────────────────────────────────────────────────────────────

widget::size_type menu::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        h.height > 0 ? h.height : s.height,
        h.width  > 0 ? h.width  : s.width
    };
}

void menu::_apply_layout()
{
    sa_->set_geometry({ 0, 0, size().width, size().height });
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void menu::on_key_press(key_event & e)
{
    if (count() == 0) return;

    switch (e.code) {
    case key::arrow_up:
        set_current(std::max(0, current_ == -1 ? count() - 1 : current_ - 1));
        break;
    case key::arrow_down:
        set_current(std::min(count() - 1, current_ == -1 ? 0 : current_ + 1));
        break;
    case key::home:
        set_current(0);
        break;
    case key::end:
        set_current(count() - 1);
        break;
    case key::page_up: {
        const int page = std::max(1, sa_->size().height);
        set_current(std::max(0, current_ - page));
        break;
    }
    case key::page_down: {
        const int page = std::max(1, sa_->size().height);
        set_current(std::min(count() - 1, current_ + page));
        break;
    }
    case key::enter:
        if (current_ >= 0) NX_EMIT(activated, current_)
        break;
    case key::printable:
        if (e.character == U' ' && current_ >= 0)
            NX_EMIT(activated, current_)
        else
            return;
        break;
    default:
        return;
    }
    e.accept();
}

} // namespace nx::tui

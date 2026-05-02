#include <nx/tui/widgets/tab_widget.hpp>
#include <nx/tui/application.hpp>

namespace nx::tui {

tab_widget::tab_widget(nx::core::object * parent)
    : widget(parent)
    , bar_(new tab_bar(this))
    , content_(new tab_layout(this))
{
    // Keep bar_ and content_ synchronised: bar_ drives the visible page.
    nx::core::connect(bar_, &tab_bar::current_changed,
                      this, [this](int idx) {
                          content_->set_current(idx);
                          NX_EMIT(current_changed, idx)
                      });
}

// ── tab management ────────────────────────────────────────────────────────────

void tab_widget::add_tab(widget * page, std::string title)
{
    content_->add_tab(page);
    bar_->add_tab(std::move(title));
    update();
}

void tab_widget::set_current(int idx)
{
    bar_->set_current(idx); // triggers bar_::current_changed → content_ + emit
}

int tab_widget::current() const noexcept { return bar_->current(); }
int tab_widget::count()   const noexcept { return bar_->count();   }

// ── layout ────────────────────────────────────────────────────────────────────

void tab_widget::_apply_layout()
{
    const int w  = size().width;
    const int h  = size().height;
    const int bh = bar_->size_hint().height; // always 1
    bar_->set_geometry({ 0, 0, w, bh });
    content_->set_geometry({ 0, bh, w, std::max(0, h - bh) });
}

} // namespace nx::tui

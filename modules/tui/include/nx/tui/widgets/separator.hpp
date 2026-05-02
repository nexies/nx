#pragma once

#include <nx/tui/widgets/widget.hpp>

namespace nx::tui {

// ── separator ─────────────────────────────────────────────────────────────────
//
// Thin decorative line.  Horizontal: fixed height=1, draws ─ across full width.
// Vertical: fixed width=1, draws │ down full height.
// Colour: theme_role::border.

class separator : public widget {
public:
    enum class orientation { horizontal, vertical };

    NX_OBJECT(separator)

    explicit separator(nx::core::object * parent = nullptr,
                       orientation o = orientation::horizontal);

    void set_orientation(orientation o);
    [[nodiscard]] orientation get_orientation() const noexcept { return orient_; }

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p) override;

private:
    orientation orient_;
};

} // namespace nx::tui

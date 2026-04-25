#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/text_align.hpp>

#include <string>

namespace nx::tui {

// ── label ─────────────────────────────────────────────────────────────────────
//
// Single-line (or multi-line) read-only text widget.
// Respects the widget's style (fg/bg/decorations) set via set_style().
// Calls fill(" ") before drawing, so the background is always painted cleanly.

class label : public widget {
    std::string text_;
    text_align  align_ = text_align::left;

public:
    NX_OBJECT(label)

    explicit label(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void                    set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_SIGNAL(text_changed, std::string)
    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Alignment ─────────────────────────────────────────────────────────────

    void                   set_alignment(text_align a);
    [[nodiscard]] text_align alignment() const noexcept { return align_; }

    NX_PROPERTY(alignment, MEMBER align_, READ alignment, WRITE set_alignment)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p) override;
};

} // namespace nx::tui

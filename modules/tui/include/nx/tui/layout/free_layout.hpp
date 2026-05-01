#pragma once

#include <nx/tui/widgets/widget.hpp>

namespace nx::tui {

// ── free_layout ───────────────────────────────────────────────────────────────
//
// Container widget where children position themselves via set_geometry().
// _apply_layout() is intentionally a no-op: the layout makes no assumptions
// about where children should sit.
//
// Use this as the parent for floating sub-windows, drag-able panels, or any
// widget whose position is managed externally (e.g. by mouse drag handlers).

class free_layout : public widget {
public:
    NX_OBJECT(free_layout)

    explicit free_layout(nx::core::object * parent = nullptr);

protected:
    // Intentional no-op: child geometries are set by the children themselves
    // or by external code (e.g. mouse drag).
    void _apply_layout() override {}
};

} // namespace nx::tui

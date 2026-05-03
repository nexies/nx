#pragma once

#include <cstdint>

namespace nx::tui {

// ── theme_role ────────────────────────────────────────────────────────────────
//
// Semantic color/style roles used by the theme system.
//
// Built-in roles occupy values [0, _count).  User-defined roles should start
// at user_base (0x8000) to avoid clashing with future built-in additions:
//
//   namespace my_app {
//       enum class role : uint32_t {
//           brand  = static_cast<uint32_t>(nx::tui::theme_role::user_base),
//           danger,
//       };
//   }
//   app->theme().set(my_app::role::brand, fg(color::rgb(255, 120, 0)));

enum class theme_role : uint32_t {
    // ── Surfaces ──────────────────────────────────────────────────────────────
    background,           // main window / widget background
    background_alt,       // alternate surface (panels, table rows, sidebars)
    foreground,           // primary text / foreground
    foreground_dim,       // secondary / muted text
    foreground_disabled,  // text on disabled elements  (carries dim decoration)

    // ── Interactive controls ──────────────────────────────────────────────────
    control,              // control at rest  (bg + fg)
    control_hover,        // control hovered
    control_active,       // control active / pressed / focused  (carries bold)
    control_disabled,     // control when the widget is disabled

    // ── Selection & accent ────────────────────────────────────────────────────
    selection,            // selected item / text background
    selection_text,       // text drawn on top of selection
    highlight,            // strong accent (focus ring, border flash, cursor)

    // ── Borders ───────────────────────────────────────────────────────────────
    border,               // default border / separator
    border_focus,         // border of the focused widget

    // ── Scrollbar ─────────────────────────────────────────────────────────────
    scrollbar,            // scrollbar track
    scrollbar_thumb,      // scrollbar thumb / grip

    // ── Status ────────────────────────────────────────────────────────────────
    success,
    warning,
    error,
    info,

    // ── Sentinel / extension point ────────────────────────────────────────────
    _count,
    user_base = 0x8000
};

} // namespace nx::tui

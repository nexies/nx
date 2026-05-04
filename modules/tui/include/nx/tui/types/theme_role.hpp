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
        background,           // main window / page surface
        background_alt,       // alternate surface (panels, sidebars, table rows)
        foreground,           // primary text on background
        foreground_dim,       // secondary / muted text
        foreground_disabled,  // text on disabled elements (carries dim decoration)

        // ── Primary brand / action ────────────────────────────────────────────────
        primary,              // primary action color (bg: buttons, links, highlights)
        primary_content,      // text drawn on top of primary background

        // ── Secondary action ──────────────────────────────────────────────────────
        secondary,            // secondary action color (bg)
        secondary_content,    // text drawn on top of secondary background

        // ── Accent ────────────────────────────────────────────────────────────────
        accent,               // accent color (focus ring, border flash, cursor blink)
        accent_content,       // text drawn on top of accent background

        // ── Neutral surface ───────────────────────────────────────────────────────
        neutral,              // neutral surface color (panels, cards)
        neutral_content,      // text on neutral surface

        // ── Interactive controls ──────────────────────────────────────────────────
        control,              // control at rest (bg + fg)
        control_hover,        // control hovered
        control_active,       // control active / pressed / focused (carries bold)
        control_disabled,     // control when the widget is disabled

        // ── Selection ────────────────────────────────────────────────────────────
        selection,            // selected item background
        selection_text,       // text drawn on top of selection

        // ── Borders ───────────────────────────────────────────────────────────────
        border,               // default border / separator
        border_focus,         // border of the focused widget

        // ── Scrollbar ─────────────────────────────────────────────────────────────
        scrollbar,            // scrollbar track
        scrollbar_thumb,      // scrollbar thumb / grip

        // ── Status ────────────────────────────────────────────────────────────────
        info,
        info_content,         // text on info background
        success,
        success_content,      // text on success background
        warning,
        warning_content,      // text on warning background
        error,
        error_content,        // text on error background

        // ── Sentinel / extension point ────────────────────────────────────────────
        _count,
        user_base = 0x8000
    };
} // namespace nx::tui

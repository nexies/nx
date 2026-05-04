#include <nx/tui/types/theme.hpp>
#include <nx/tui/types/color.hpp>

namespace nx::tui {

// ── helpers (file-local) ──────────────────────────────────────────────────────

namespace {

constexpr std::size_t idx(theme_role r) noexcept
{
    return static_cast<std::size_t>(r);
}

} // anonymous namespace

// ── construction ──────────────────────────────────────────────────────────────

theme::theme() = default;

// ── access ────────────────────────────────────────────────────────────────────

const style_option & theme::get_style(theme_role r) const noexcept
{
    static const style_option empty;
    const std::size_t i = idx(r);
    if (i >= k_builtin_count) return empty;
    return builtin_[i];
}

style_option theme::get_style(uint32_t r) const noexcept
{
    if (r < static_cast<uint32_t>(theme_role::_count))
        return get_style(static_cast<theme_role>(r));

    const auto it = custom_.find(r);
    return it != custom_.end() ? it->second : style_option{};
}

color theme::get_color(theme_role r) const noexcept
{
    return get_style(r).foreground.value_or(color::default_color);
}

color theme::get_bg(theme_role r) const noexcept
{
    return get_style(r).background.value_or(color::default_color);
}

// ── mutation ──────────────────────────────────────────────────────────────────

theme & theme::set(theme_role r, style_option s) noexcept
{
    const std::size_t i = idx(r);
    if (i < k_builtin_count)
        builtin_[i] = std::move(s);
    return *this;
}

theme & theme::set(uint32_t r, style_option s) noexcept
{
    if (r < static_cast<uint32_t>(theme_role::_count))
        return set(static_cast<theme_role>(r), std::move(s));
    custom_[r] = std::move(s);
    return *this;
}

theme & theme::set_color(theme_role r, color c) noexcept
{
    const std::size_t i = idx(r);
    if (i < k_builtin_count) builtin_[i].foreground = c;
    return *this;
}

theme & theme::set_bg(theme_role r, color c) noexcept
{
    const std::size_t i = idx(r);
    if (i < k_builtin_count) builtin_[i].background = c;
    return *this;
}

theme & theme::set_colors(theme_role r, color fg_c, color bg_c) noexcept
{
    const std::size_t i = idx(r);
    if (i < k_builtin_count) {
        builtin_[i].foreground = fg_c;
        builtin_[i].background = bg_c;
    }
    return *this;
}

// ── dark theme ────────────────────────────────────────────────────────────────
//
// Inspired by Catppuccin Mocha.

theme theme::dark()
{
    theme t;
    using r = theme_role;

    // Surfaces
    t.set(r::background,          bg(color::rgb( 30,  30,  46)) | fg(color::rgb(205, 214, 244)));
    t.set(r::background_alt,      bg(color::rgb( 24,  24,  37)) | fg(color::rgb(205, 214, 244)));
    t.set(r::foreground,          fg(color::rgb(205, 214, 244)));
    t.set(r::foreground_dim,      fg(color::rgb(127, 132, 156)));
    t.set(r::foreground_disabled, fg(color::rgb( 69,  71,  90)) | dim());

    // Primary — blue
    t.set(r::primary,         bg(color::rgb(137, 180, 250)) | fg(color::rgb( 17,  17,  27)));
    t.set(r::primary_content, fg(color::rgb( 17,  17,  27)));

    // Secondary — pink
    t.set(r::secondary,         bg(color::rgb(245, 194, 231)) | fg(color::rgb( 17,  17,  27)));
    t.set(r::secondary_content, fg(color::rgb( 17,  17,  27)));

    // Accent — mauve (focus ring, border flash, cursor)
    t.set(r::accent,         fg(color::rgb(203, 166, 247)));
    t.set(r::accent_content, fg(color::rgb( 17,  17,  27)));

    // Neutral surface
    t.set(r::neutral,         bg(color::rgb( 49,  50,  68)) | fg(color::rgb(205, 214, 244)));
    t.set(r::neutral_content, fg(color::rgb(205, 214, 244)));

    // Controls
    t.set(r::control,          bg(color::rgb( 49,  50,  68)) | fg(color::rgb(205, 214, 244)));
    t.set(r::control_hover,    bg(color::rgb( 69,  71,  90)) | fg(color::rgb(205, 214, 244)));
    t.set(r::control_active,   bg(color::rgb( 88,  91, 112)) | fg(color::rgb(205, 214, 244)) | bold());
    t.set(r::control_disabled, bg(color::rgb( 24,  24,  37)) | fg(color::rgb( 69,  71,  90)));

    // Selection
    t.set(r::selection,      bg(color::rgb(137, 180, 250)) | fg(color::rgb( 30,  30,  46)));
    t.set(r::selection_text, fg(color::rgb( 30,  30,  46)) | bg(color::rgb(137, 180, 250)));

    // Borders
    t.set(r::border,       fg(color::rgb( 69,  71,  90)));
    t.set(r::border_focus, fg(color::rgb(203, 166, 247)));

    // Scrollbar
    t.set(r::scrollbar,       bg(color::rgb( 30,  30,  46)) | fg(color::rgb( 49,  50,  68)));
    t.set(r::scrollbar_thumb, bg(color::rgb( 88,  91, 112)) | fg(color::rgb(127, 132, 156)));

    // Status
    t.set(r::info,            fg(color::rgb(137, 180, 250)));
    t.set(r::info_content,    fg(color::rgb( 17,  17,  27)));
    t.set(r::success,         fg(color::rgb(166, 227, 161)));
    t.set(r::success_content, fg(color::rgb( 17,  17,  27)));
    t.set(r::warning,         fg(color::rgb(249, 226, 175)));
    t.set(r::warning_content, fg(color::rgb( 17,  17,  27)));
    t.set(r::error,           fg(color::rgb(243, 139, 168)));
    t.set(r::error_content,   fg(color::rgb( 17,  17,  27)));

    return t;
}

// ── light theme ───────────────────────────────────────────────────────────────
//
// Inspired by Catppuccin Latte.

theme theme::light()
{
    theme t;
    using r = theme_role;

    // Surfaces
    t.set(r::background,          bg(color::rgb(239, 241, 245)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::background_alt,      bg(color::rgb(230, 233, 239)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::foreground,          fg(color::rgb( 76,  79, 105)));
    t.set(r::foreground_dim,      fg(color::rgb(156, 160, 176)));
    t.set(r::foreground_disabled, fg(color::rgb(188, 192, 204)) | dim());

    // Primary — blue
    t.set(r::primary,         bg(color::rgb( 30, 102, 245)) | fg(color::rgb(239, 241, 245)));
    t.set(r::primary_content, fg(color::rgb(239, 241, 245)));

    // Secondary — pink
    t.set(r::secondary,         bg(color::rgb(234, 118, 203)) | fg(color::rgb(239, 241, 245)));
    t.set(r::secondary_content, fg(color::rgb(239, 241, 245)));

    // Accent — mauve (focus ring, border flash, cursor)
    t.set(r::accent,         fg(color::rgb(136,  57, 239)));
    t.set(r::accent_content, fg(color::rgb(239, 241, 245)));

    // Neutral surface
    t.set(r::neutral,         bg(color::rgb(204, 208, 218)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::neutral_content, fg(color::rgb( 76,  79, 105)));

    // Controls
    t.set(r::control,          bg(color::rgb(204, 208, 218)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::control_hover,    bg(color::rgb(188, 192, 204)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::control_active,   bg(color::rgb(172, 176, 190)) | fg(color::rgb( 76,  79, 105)) | bold());
    t.set(r::control_disabled, bg(color::rgb(230, 233, 239)) | fg(color::rgb(188, 192, 204)));

    // Selection
    t.set(r::selection,      bg(color::rgb( 30, 102, 245)) | fg(color::rgb(239, 241, 245)));
    t.set(r::selection_text, fg(color::rgb(239, 241, 245)) | bg(color::rgb( 30, 102, 245)));

    // Borders
    t.set(r::border,       fg(color::rgb(188, 192, 204)));
    t.set(r::border_focus, fg(color::rgb(136,  57, 239)));

    // Scrollbar
    t.set(r::scrollbar,       bg(color::rgb(239, 241, 245)) | fg(color::rgb(204, 208, 218)));
    t.set(r::scrollbar_thumb, bg(color::rgb(156, 160, 176)) | fg(color::rgb( 76,  79, 105)));

    // Status
    t.set(r::info,            fg(color::rgb( 30, 102, 245)));
    t.set(r::info_content,    fg(color::rgb(239, 241, 245)));
    t.set(r::success,         fg(color::rgb( 64, 160,  43)));
    t.set(r::success_content, fg(color::rgb(239, 241, 245)));
    t.set(r::warning,         fg(color::rgb(223, 142,  29)));
    t.set(r::warning_content, fg(color::rgb(239, 241, 245)));
    t.set(r::error,           fg(color::rgb(210,  15,  57)));
    t.set(r::error_content,   fg(color::rgb(239, 241, 245)));

    return t;
}

// ── halloween theme ───────────────────────────────────────────────────────────
//
// Ported from DaisyUI "halloween" theme (OKLCH → sRGB).

theme theme::halloween()
{
    theme t;
    using r  = theme_role;
    using c  = color;
    auto ok  = [](float L, float C, float H) { return c::from_oklch(L, C, H); };

    // DaisyUI base variables
    const color base100    = ok(0.21f, 0.006f,  56.043f); // dark brownish
    const color base200    = ok(0.14f, 0.004f,  49.250f); // darker
    const color base300    = ok(0.00f, 0.000f,   0.000f); // black
    const color basecont   = ok(0.85f, 0.000f,   0.000f); // near-white
    const color primc      = ok(0.77f, 0.204f,  60.620f); // orange
    const color primcont   = ok(0.20f, 0.004f, 196.779f); // dark teal
    const color secc       = ok(0.46f, 0.248f, 305.030f); // purple
    const color seccont    = ok(0.89f, 0.049f, 305.030f); // light purple
    const color accc       = ok(0.65f, 0.223f, 136.073f); // lime green
    const color acccont    = ok(0.00f, 0.000f,   0.000f); // black
    const color neut       = ok(0.24f, 0.046f,  65.681f); // dark brownish
    const color neutcont   = ok(0.85f, 0.009f,  65.681f); // light tan

    // Surfaces
    t.set(r::background,          bg(base100) | fg(basecont));
    t.set(r::background_alt,      bg(base200) | fg(basecont));
    t.set(r::foreground,          fg(basecont));
    t.set(r::foreground_dim,      fg(ok(0.60f, 0.004f, 56.0f)));
    t.set(r::foreground_disabled, fg(ok(0.40f, 0.004f, 56.0f)) | dim());

    // Primary — orange
    t.set(r::primary,         bg(primc)    | fg(primcont));
    t.set(r::primary_content, fg(primcont));

    // Secondary — purple
    t.set(r::secondary,         bg(secc)    | fg(seccont));
    t.set(r::secondary_content, fg(seccont));

    // Accent — lime green
    t.set(r::accent,         fg(accc));
    t.set(r::accent_content, fg(acccont));

    // Neutral surface
    t.set(r::neutral,         bg(neut)     | fg(neutcont));
    t.set(r::neutral_content, fg(neutcont));

    // Controls (derived from neutral)
    t.set(r::control,          bg(neut)    | fg(neutcont));
    t.set(r::control_hover,    bg(ok(0.30f, 0.046f, 65.681f)) | fg(neutcont));
    t.set(r::control_active,   bg(primc)   | fg(primcont) | bold());
    t.set(r::control_disabled, bg(base200) | fg(ok(0.40f, 0.004f, 56.0f)));

    // Selection — primary as selection color
    t.set(r::selection,      bg(primc)    | fg(primcont));
    t.set(r::selection_text, fg(primcont) | bg(primc));

    // Borders — neutral for default, orange for focus
    t.set(r::border,       fg(neut));
    t.set(r::border_focus, fg(primc));

    // Scrollbar
    t.set(r::scrollbar,       bg(base100) | fg(neut));
    t.set(r::scrollbar_thumb, bg(neut)    | fg(neutcont));

    // Status (DaisyUI values)
    const color info_c    = ok(0.546f, 0.215f, 262.88f);
    const color info_co   = ok(0.909f, 0.043f, 262.88f);
    const color succ_c    = ok(0.627f, 0.169f, 149.213f);
    const color succ_co   = ok(0.125f, 0.033f, 149.213f);
    const color warn_c    = ok(0.666f, 0.157f,  58.318f);
    const color warn_co   = ok(0.133f, 0.031f,  58.318f);
    const color err_c     = ok(0.657f, 0.199f,  27.330f);
    const color err_co    = ok(0.131f, 0.039f,  27.330f);

    t.set(r::info,            fg(info_c));
    t.set(r::info_content,    fg(info_co));
    t.set(r::success,         fg(succ_c));
    t.set(r::success_content, fg(succ_co));
    t.set(r::warning,         fg(warn_c));
    t.set(r::warning_content, fg(warn_co));
    t.set(r::error,           fg(err_c));
    t.set(r::error_content,   fg(err_co));

    return t;
}

} // namespace nx::tui

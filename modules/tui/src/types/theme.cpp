#include <nx/tui/types/theme.hpp>

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

    // Controls
    t.set(r::control,          bg(color::rgb( 49,  50,  68)) | fg(color::rgb(205, 214, 244)));
    t.set(r::control_hover,    bg(color::rgb( 69,  71,  90)) | fg(color::rgb(205, 214, 244)));
    t.set(r::control_active,   bg(color::rgb( 88,  91, 112)) | fg(color::rgb(205, 214, 244)) | bold());
    t.set(r::control_disabled, bg(color::rgb( 24,  24,  37)) | fg(color::rgb( 69,  71,  90)));

    // Selection & accent
    t.set(r::selection,      bg(color::rgb(137, 180, 250)) | fg(color::rgb( 30,  30,  46)));
    t.set(r::selection_text, fg(color::rgb( 30,  30,  46)) | bg(color::rgb(137, 180, 250)));
    t.set(r::highlight,      fg(color::rgb(203, 166, 247)));

    // Borders
    t.set(r::border,       fg(color::rgb( 69,  71,  90)));
    t.set(r::border_focus, fg(color::rgb(203, 166, 247)));

    // Scrollbar
    t.set(r::scrollbar,       bg(color::rgb( 30,  30,  46)) | fg(color::rgb( 49,  50,  68)));
    t.set(r::scrollbar_thumb, bg(color::rgb( 88,  91, 112)) | fg(color::rgb(127, 132, 156)));

    // Status
    t.set(r::success, fg(color::rgb(166, 227, 161)));
    t.set(r::warning, fg(color::rgb(249, 226, 175)));
    t.set(r::error,   fg(color::rgb(243, 139, 168)));
    t.set(r::info,    fg(color::rgb(137, 180, 250)));

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

    // Controls
    t.set(r::control,          bg(color::rgb(204, 208, 218)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::control_hover,    bg(color::rgb(188, 192, 204)) | fg(color::rgb( 76,  79, 105)));
    t.set(r::control_active,   bg(color::rgb(172, 176, 190)) | fg(color::rgb( 76,  79, 105)) | bold());
    t.set(r::control_disabled, bg(color::rgb(230, 233, 239)) | fg(color::rgb(188, 192, 204)));

    // Selection & accent
    t.set(r::selection,      bg(color::rgb( 30, 102, 245)) | fg(color::rgb(239, 241, 245)));
    t.set(r::selection_text, fg(color::rgb(239, 241, 245)) | bg(color::rgb( 30, 102, 245)));
    t.set(r::highlight,      fg(color::rgb(136,  57, 239)));

    // Borders
    t.set(r::border,       fg(color::rgb(188, 192, 204)));
    t.set(r::border_focus, fg(color::rgb(136,  57, 239)));

    // Scrollbar
    t.set(r::scrollbar,       bg(color::rgb(239, 241, 245)) | fg(color::rgb(204, 208, 218)));
    t.set(r::scrollbar_thumb, bg(color::rgb(156, 160, 176)) | fg(color::rgb( 76,  79, 105)));

    // Status
    t.set(r::success, fg(color::rgb( 64, 160,  43)));
    t.set(r::warning, fg(color::rgb(223, 142,  29)));
    t.set(r::error,   fg(color::rgb(210,  15,  57)));
    t.set(r::info,    fg(color::rgb( 30, 102, 245)));

    return t;
}

} // namespace nx::tui

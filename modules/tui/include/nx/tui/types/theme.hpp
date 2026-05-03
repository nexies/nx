#pragma once

#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/types/style_option.hpp>

#include <array>
#include <unordered_map>

namespace nx::tui {
    // ── theme ─────────────────────────────────────────────────────────────────────
    //
    // Maps semantic theme_role values to style_option entries (fg color, bg color,
    // and optional text decorations such as bold or dim).
    //
    // Built-in roles are stored in a fixed array for O(1) access.
    // User-defined roles (>= theme_role::user_base) go into an unordered_map.
    //
    // Usage — built-in roles:
    //   theme t = theme::dark();
    //   t.set(theme_role::highlight, fg(color::rgb(255, 120, 0)));
    //
    // Usage — custom roles:
    //   namespace my_app {
    //       enum class role : uint32_t {
    //           brand = static_cast<uint32_t>(nx::tui::theme_role::user_base),
    //       };
    //   }
    //   t.set(my_app::role::brand, fg(color::rgb(0, 150, 255)) | bold());
    //
    // Accessing:
    //   style_option s = t.get_style(theme_role::control_hover);
    //   color        c = t.get_color(theme_role::foreground);  // fg
    //   color        b = t.get_bg   (theme_role::background);  // bg

    class theme {
    public:
        theme();

        // ── Primary access ────────────────────────────────────────────────────────

        // Returns the full style_option for a built-in role.
        NX_NODISCARD const style_option &
        get_style(theme_role r) const noexcept;

        // Returns the full style_option for any role (built-in or custom).
        // Returns an empty style_option for unknown custom roles.
        NX_NODISCARD style_option
        get_style(uint32_t r) const noexcept;

        // Convenience: foreground color of the role's style.
        NX_NODISCARD color
        get_color(theme_role r) const noexcept;

        // Convenience: background color of the role's style.
        NX_NODISCARD color
        get_bg(theme_role r) const noexcept;

        // ── Mutation ──────────────────────────────────────────────────────────────

        theme &
        set(theme_role r, style_option s) noexcept;

        // Generic overload for user-defined roles (any uint32_t-convertible enum).
        template<typename R>
        theme &
        set(R r, style_option s) noexcept {
            return set(static_cast<uint32_t>(r), std::move(s));
        }

        theme &
        set(uint32_t r, style_option s) noexcept;

        // Convenience setters (only touch the fg / bg of an existing entry).
        theme &
        set_color(theme_role r, color fg) noexcept;

        theme &
        set_bg(theme_role r, color bg) noexcept;

        theme &
        set_colors(theme_role r, color fg, color bg) noexcept;

        // Generic getter for user-defined roles.
        template<typename R>
        NX_NODISCARD style_option
        get_style(R r) const noexcept {
            return get_style(static_cast<uint32_t>(r));
        }

        template<typename R>
        NX_NODISCARD color
        get_color(R r) const noexcept {
            return get_style(r).foreground.value_or(color::default_color);
        }

        template<typename R>
        NX_NODISCARD color
        get_bg(R r) const noexcept {
            return get_style(r).background.value_or(color::default_color);
        }

        // ── Built-in themes ───────────────────────────────────────────────────────

        NX_NODISCARD static
        theme dark();

        NX_NODISCARD static
        theme light();

    private:
        static constexpr std::size_t k_builtin_count =
                static_cast<std::size_t>(theme_role::_count);

        std::array<style_option, k_builtin_count> builtin_;
        std::unordered_map<uint32_t, style_option> custom_;
    };
} // namespace nx::tui

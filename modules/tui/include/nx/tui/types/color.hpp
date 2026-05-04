#pragma once

#include <cstdint>
#include <string>
#include <ostream>

#include <nx/common/helpers.hpp>

namespace nx::tui {

    enum class color_type : uint8_t {
        palette1   = 0x10,
        palette16  = 0x20,
        palette256 = 0x40,
        true_color = 0x80,
    };

    class color
    {
    public:
        // ── Named constants ───────────────────────────────────────────────────
        // "default_color" means "use the terminal's default" (no explicit color).
        static const color default_color;
        static const color black;
        static const color red;
        static const color green;
        static const color yellow;
        static const color blue;
        static const color magenta;
        static const color cyan;
        static const color gray_light;
        static const color gray_dark;
        static const color red_bright;
        static const color green_bright;
        static const color yellow_bright;
        static const color blue_bright;
        static const color magenta_bright;
        static const color cyan_bright;
        static const color white;

        // ── Factory methods ───────────────────────────────────────────────────
        static color rgb(uint8_t r, uint8_t g, uint8_t b);
        static color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        static color from_palette16(uint32_t index);
        static color from_palette256(uint32_t index);
        // Convert OKLCH (L in [0,1], C chroma, H in degrees) to sRGB.
        static color from_oklch(float L, float C, float H_deg);

        static color interpolate(float t, const color & a, const color & b);
        static color scale    (const color & c, float factor);
        static color blend    (const color & a, const color & b);
        static color negative (const color & c);

        // ── Constructors ──────────────────────────────────────────────────────
        color();
        color(uint8_t r, uint8_t g, uint8_t b);
        color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

        // ── Accessors ─────────────────────────────────────────────────────────
        NX_NODISCARD constexpr color_type type() const noexcept { return type_; }
        NX_NODISCARD uint8_t r() const noexcept;
        NX_NODISCARD uint8_t g() const noexcept;
        NX_NODISCARD uint8_t b() const noexcept;
        NX_NODISCARD uint8_t a() const noexcept;

        NX_NODISCARD std::string name()     const;
        // Returns the ANSI escape sequence to set this color.
        // Pass background=true for background color sequence.
        NX_NODISCARD std::string to_ansi(bool background = false) const;

        // ── Comparison ────────────────────────────────────────────────────────
        NX_NODISCARD bool
        operator==(const color & o) const noexcept
        { return r() == o.r() && g() == o.g() && b() == o.b(); }

        NX_NODISCARD bool
        operator!=(const color & o) const noexcept
        { return !(*this == o); }

    private:
        color_type type_  = color_type::palette1;
        uint32_t   value_ = 100;

        color(color_type type, uint32_t value);
    };

} // namespace nx::tui

inline std::ostream &
operator<<(std::ostream & os, const nx::tui::color & c)
{
    return os << c.to_ansi();
}

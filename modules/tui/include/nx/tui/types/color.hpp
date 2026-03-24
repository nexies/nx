//
// Created by nexie on 17.03.2026.
//

#ifndef NX_COLOR_HPP
#define NX_COLOR_HPP

#include <cstdint>
#include <string>
#include <nx/core/detail/enum_defs.hpp>

namespace nx::tui
{
    enum class ColorType : uint8_t
    {
        Palette1 = 0x10,
        Palette16 = 0x20,
        Palette256 = 0x40,
        TrueColor = 0x80,
    };

    class Color
    {
    public:
        const static Color Default;
        const static Color Black;
        const static Color Red;
        const static Color Green;
        const static Color Yellow;
        const static Color Blue;
        const static Color Magenta;
        const static Color Cyan;
        const static Color GrayLight;
        const static Color GrayDark;
        const static Color RedLight;
        const static Color GreenLight;
        const static Color YellowLight;
        const static Color BlueLight;
        const static Color MagentaLight;
        const static Color CyanLight;
        const static Color White;

        static Color RGB (uint8_t red, uint8_t green, uint8_t blue);
        static Color RGBA (uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
        static Color FromPalette16 (uint32_t value);
        static Color FromPalette256 (uint32_t value);

        static Color Interpolate (float t, const Color & color1, const Color & color2);
        static Color Blend (const Color & color1, const Color & color2);
        static Color Negative (const Color & color);

    private:
        ColorType type_ = ColorType::Palette1;
        uint32_t value_ = 100;

        Color(ColorType type, uint32_t value);
    public:
        Color();
        Color(uint8_t red, uint8_t green, uint8_t blue);
        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

        [[nodiscard]]
        std::string colorName () const;

        [[nodiscard]] constexpr ColorType
        type() const;

        [[nodiscard]] constexpr uint8_t
        red () const;

        [[nodiscard]] constexpr uint8_t
        green () const;

        [[nodiscard]] constexpr uint8_t
        blue () const;

        [[nodiscard]] constexpr uint8_t
        alpha () const;

        [[nodiscard]] std::string
        print (bool background = false) const;

        [[nodiscard]] constexpr bool
        operator == (const Color & other) const
        {
            return (red() == other.red() && green() == other.green() && blue() == other.blue());
        }

        [[nodiscard]] constexpr bool
        operator != (const Color & other) const
        {
            return !(*this == other);
        }
    };
}

inline std::ostream &
operator << (std::ostream & os, const nx::tui::Color & c)
{
    return os << c.print();
}

#endif //NX_COLOR_HPP
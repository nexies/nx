//
// Created by nexie on 18.03.2026.
//

#include "nx/tui/types/color.hpp"
#include "nx/tui/types/color.hpp"
#include "nx/tui/types/color.hpp"

#include <nx/tui/types/color.hpp>

#include <nx/tui/terminal/Terminal.hpp>

namespace
{
    constexpr uint32_t g_8bit_mask = 0b0000'0000'1111'1111;

    constexpr uint32_t g_true_red_shift = 24;
    constexpr uint32_t g_true_green_shift = 16;
    constexpr uint32_t g_true_blue_shift = 8;
    constexpr uint32_t g_true_alpha_shift = 0;

    constexpr uint32_t ratio_255_to_6 = 256 / 6;
    constexpr uint32_t palette256_shades_of_gray_count = 24;

    constexpr uint32_t
    to_rgb_helper (uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        return static_cast<uint32_t>(red) << g_true_red_shift |
            static_cast<uint32_t>(green) << g_true_green_shift |
            static_cast<uint32_t>(blue) << g_true_blue_shift |
            static_cast<uint32_t>(red) << g_true_alpha_shift;
    }

    constexpr uint32_t
    to_rgb_helper (uint8_t red, uint8_t green, uint8_t blue)
    {
        return static_cast<uint32_t>(red) << g_true_red_shift |
            static_cast<uint32_t>(green) << g_true_green_shift |
            static_cast<uint32_t>(blue) << g_true_blue_shift;
    }

    constexpr uint8_t
    from_rgb_helper (uint32_t value, uint32_t shift)
    {
        return static_cast<uint8_t>((value >> shift) & g_8bit_mask);
    }

    constexpr uint8_t
    red_from_rgb_helper (uint32_t value)
    {
        return from_rgb_helper (value, g_true_red_shift);
    }

    constexpr uint8_t
    green_from_rgb_helper (uint32_t value)
    {
        return from_rgb_helper (value, g_true_green_shift);
    }

    constexpr uint8_t
    blue_from_rgb_helper (uint32_t value)
    {
        return from_rgb_helper (value, g_true_blue_shift);
    }

    constexpr uint8_t
    alpha_from_rgb_helper (uint32_t value)
    {
        return from_rgb_helper (value, g_true_alpha_shift);
    }

    const std::vector<uint32_t> palette16_to_rgb =
    {
        to_rgb_helper(0,   0,   0), // black
        to_rgb_helper(205, 0,   0), // red
        to_rgb_helper(0,   205, 0), // green
        to_rgb_helper(205, 205, 0), // yellow
        to_rgb_helper(0,   0,   238), // blue
        to_rgb_helper(205, 0,   205), // magenta
        to_rgb_helper(0,   205, 205), // cyan
        to_rgb_helper(229, 229, 229), // white

        to_rgb_helper(127, 127, 127), // bright black
        to_rgb_helper(255, 0,   0), // bright red
        to_rgb_helper(0,   255, 0), // bright green
        to_rgb_helper(255, 255, 0), // bright yellow
        to_rgb_helper(92,  92,  255), // bright blue
        to_rgb_helper(255, 0, 255), // bright magenta
        to_rgb_helper(0,   255, 255), // bright cyan
        to_rgb_helper(255, 255, 255), // bright white
    };

    constexpr uint32_t
    rgb_to_palette (uint8_t red, uint8_t green, uint8_t blue)
    {
        const auto dred = red * 6 / 256;
        const auto dgreen = green * 6 / 256;
        const auto dblue = blue * 6 / 256;

        const auto to_256_code = [] (uint8_t red, uint8_t green, uint8_t blue)
        {
            return red * 36 + green * 6 + blue + 16;
        };

        // shade of gray
        if (dred == dgreen && dgreen == dblue)
        {
            auto level = red * 24 / 256;
            return 232 + level;
        }

        // 6x6x6 rgb cube
        return to_256_code (dred, dgreen, dblue);
    }

    constexpr uint32_t
    rgb_from_palette(uint32_t value)
    {
        //TODO: need a color table for converting Palette16 to RGB
        if (value < 16)
            return palette16_to_rgb[value];

        if (value < 232)
        {
            auto n = value - 16;
            auto r = n / 36;
            auto g = (n % 36) / 6;
            auto b = n % 6;
            return to_rgb_helper(r * 256 / 6, g * 256 / 6, b * 256 / 6);
        }

        if (value < 256)
        {
            const auto level = value - 232;
            return to_rgb_helper(value * 256 / 24, value * 256 / 24,value * 256 / 24);
        }

        return 0;
    }
}

namespace nx::tui
{
    constexpr ColorType Color::type() const
    {
        return type_;
    }

    constexpr uint8_t Color::red() const
    {
        switch (type_)
        {
        case ColorType::TrueColor:
            return red_from_rgb_helper (value_);
        case ColorType::Palette256:
        case ColorType::Palette16:
            {
            auto rgb = rgb_from_palette(value_);
            return red_from_rgb_helper (rgb);
            }
        default:
            return 0;
        }
    }

    constexpr uint8_t Color::green() const
    {
        switch (type_)
        {
        case ColorType::TrueColor:
            return green_from_rgb_helper (value_);
        case ColorType::Palette256:
        case ColorType::Palette16:
            {
                auto rgb = rgb_from_palette(value_);
                return green_from_rgb_helper (rgb);
            }
        default:
            return 0;
        }
    }

    constexpr uint8_t Color::blue() const
    {
        switch (type_)
        {
        case ColorType::TrueColor:
            return blue_from_rgb_helper (value_);
        case ColorType::Palette256:
        case ColorType::Palette16:
            {
                auto rgb = rgb_from_palette(value_);
                return blue_from_rgb_helper (rgb);
            }
        default:
            return 0;
        }
    }

    constexpr uint8_t Color::alpha() const
    {
        if (type_ == ColorType::TrueColor)
            return alpha_from_rgb_helper (value_);
        else
            return 255;
    }

    std::string Color::print(bool background) const
    {
        switch (type_)
        {
        case ColorType::Palette1: return "\x1b[0m";
        case ColorType::Palette16:
        case ColorType::Palette256:
            {
                if (background)
                    return fmt::format("\x1b[48;5;{}m", value_);
                return fmt::format("\x1b[38;5;{}m", value_);
            }
        case ColorType::TrueColor:
            {
                return fmt::format("\x1b[{};2;{};{};{}m", background ? 48 : 38, red(), green(), blue());
            }
        default:
            return "<INVALID_COLOR>";
        }
    }

    const Color Color::Default      (ColorType::Palette1, 0);
    const Color Color::Black        (ColorType::Palette16, 0);
    const Color Color::Red          (ColorType::Palette16, 1);
    const Color Color::Green        (ColorType::Palette16, 2);
    const Color Color::Yellow       (ColorType::Palette16, 3);
    const Color Color::Blue         (ColorType::Palette16, 4);
    const Color Color::Magenta      (ColorType::Palette16, 5);
    const Color Color::Cyan         (ColorType::Palette16, 6);
    const Color Color::GrayLight    (ColorType::Palette16, 7);
    const Color Color::GrayDark     (ColorType::Palette16, 8);
    const Color Color::RedLight     (ColorType::Palette16, 9);
    const Color Color::GreenLight   (ColorType::Palette16, 10);
    const Color Color::YellowLight  (ColorType::Palette16, 11);
    const Color Color::BlueLight    (ColorType::Palette16, 12);
    const Color Color::MagentaLight (ColorType::Palette16, 13);
    const Color Color::CyanLight    (ColorType::Palette16, 14);
    const Color Color::White        (ColorType::Palette16, 15);

    Color Color::RGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        return Color(red, green, blue);
    }

    Color Color::RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        return Color(red, green, blue, alpha);
    }

    Color Color::FromPalette16(uint32_t value)
    {
        return Color(ColorType::Palette16, value);
    }

    Color Color::FromPalette256(uint32_t value)
    {
        return Color(ColorType::Palette256, value);
    }

    Color Color::Interpolate(float t, const Color& color1, const Color& color2)
    {
        auto interp = [t](uint8_t a_u, uint8_t b_u) {
            constexpr float gamma = 2.2F;
            const float a_f = powf(a_u, gamma);
            const float b_f = powf(b_u, gamma);
            const float c_f = a_f * (1.0F - t) +  //
                              b_f * t;
            return static_cast<uint8_t>(powf(c_f, 1.F / gamma));
        };

        return RGB(interp(color1.red(), color2.red()),
            interp(color1.green(), color2.green()),
            interp(color1.blue(), color2.blue()));
    }

    Color Color::Blend(const Color& color1, const Color& color2)
    {
        return Interpolate(color1.alpha() / 255.f, color1, color2);
    }

    Color Color::Negative(const Color& color)
    {
        return RGBA(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
    }

    Color::Color() :
        Color(ColorType::Palette1, 0)
    {
    }

    Color::Color(ColorType type, uint32_t value)
        : type_ { type }
    , value_ { value }
    { }

    // Color::Color(Palette1 value) :
    // type_ { ColorType::Palette1 },
    // value_ { value }
    // {

    // }

    // Color::Color(Palette16 value)
    // : type_ { ColorType::Palette16 }
    // , value_ { value }
    // {

    // }

    // Color::Color(Palette256 value)
    // : type_ { ColorType::Palette256 }
    // , value_ { value }
    // {

    // }

    Color::Color(uint8_t red, uint8_t green, uint8_t blue)
        : type_ { Terminal::GetColorSupport() >= ColorType::TrueColor
            ? ColorType::TrueColor : ColorType::Palette256 }
    , value_ { type_ >= ColorType::TrueColor
        ? to_rgb_helper(red, green, blue) : rgb_to_palette(red, green, blue) }
    {

    }

    Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
        : type_ { Terminal::GetColorSupport() >= ColorType::TrueColor
            ? ColorType::TrueColor : ColorType::Palette256 }
    , value_ { type_ >= ColorType::TrueColor
        ? to_rgb_helper(red, green, blue, alpha) : rgb_to_palette(red, green, blue)}
    {

    }

    std::string Color::colorName() const
    {
        switch (type_)
        {
        case ColorType::Palette1:
            return fmt::format("Palette1::Default");
        case ColorType::Palette16:
            return fmt::format("Palette16::{}", (value_));
        case ColorType::Palette256:
            {
                if (value_ < 16) return fmt::format("Palette256::{}", (value_));
                if (value_ < 232)
                {
                    auto n = value_ - 16;
                    auto r = n / 36;
                    auto g = (n % 36) / 6;
                    auto b = n % 6;
                    return fmt::format("Palette256::rgb({}, {}, {})", r, g, b);
                }
                if (value_ < 256) return fmt::format("Palette256::gray({})", value_ - 232);

                return fmt::format("Palette256::INVALID_COLOR");
            }
        case ColorType::TrueColor:
            return fmt::format("RGBA::#{:x}", value_);
        default:
            return "invalid color";
        }
    }
}

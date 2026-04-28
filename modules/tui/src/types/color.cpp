#include <vector>
#include <cmath>
#include <fmt/format.h>

#include <nx/common/platform.hpp>
#include <nx/tui/types/color.hpp>
#include <nx/tui/terminal/terminal.hpp>

namespace
{
    // pow_f is not in the std namespace on MinGW; std::pow has a float
    // overload on all platforms.
    inline float pow_f(float base, float exp) noexcept
    {
#if defined(NX_OS_WINDOWS)
        return static_cast<float>(std::pow(base, exp));
#else
        return powf(base, exp);
#endif
    }

    constexpr uint32_t g_8bit_mask         = 0b0000'0000'1111'1111;
    constexpr uint32_t g_true_red_shift    = 24;
    constexpr uint32_t g_true_green_shift  = 16;
    constexpr uint32_t g_true_blue_shift   = 8;
    constexpr uint32_t g_true_alpha_shift  = 0;

    constexpr uint32_t
    pack_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
    {
        return (uint32_t(r) << g_true_red_shift)
             | (uint32_t(g) << g_true_green_shift)
             | (uint32_t(b) << g_true_blue_shift)
             | (uint32_t(a) << g_true_alpha_shift);
    }

    constexpr uint32_t
    pack_rgb(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        return (uint32_t(r) << g_true_red_shift)
             | (uint32_t(g) << g_true_green_shift)
             | (uint32_t(b) << g_true_blue_shift);
    }

    constexpr uint8_t
    unpack(uint32_t value, uint32_t shift) noexcept
    {
        return static_cast<uint8_t>((value >> shift) & g_8bit_mask);
    }

    const std::vector<uint32_t> g_palette16_rgb = {
        pack_rgb(  0,   0,   0), // 0  black
        pack_rgb(205,   0,   0), // 1  red
        pack_rgb(  0, 205,   0), // 2  green
        pack_rgb(205, 205,   0), // 3  yellow
        pack_rgb(  0,   0, 238), // 4  blue
        pack_rgb(205,   0, 205), // 5  magenta
        pack_rgb(  0, 205, 205), // 6  cyan
        pack_rgb(229, 229, 229), // 7  light gray
        pack_rgb(127, 127, 127), // 8  dark gray
        pack_rgb(255,   0,   0), // 9  bright red
        pack_rgb(  0, 255,   0), // 10 bright green
        pack_rgb(255, 255,   0), // 11 bright yellow
        pack_rgb( 92,  92, 255), // 12 bright blue
        pack_rgb(255,   0, 255), // 13 bright magenta
        pack_rgb(  0, 255, 255), // 14 bright cyan
        pack_rgb(255, 255, 255), // 15 white
    };

    constexpr uint32_t
    rgb_to_palette256(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        const auto dr = r * 6u / 256u;
        const auto dg = g * 6u / 256u;
        const auto db = b * 6u / 256u;

        if (dr == dg && dg == db) {
            return 232u + r * 24u / 256u; // grayscale ramp
        }
        return 16u + dr * 36u + dg * 6u + db; // 6x6x6 cube
    }

    constexpr uint32_t
    palette256_to_rgb(uint32_t index) noexcept
    {
        if (index < 16)
            return g_palette16_rgb[index];

        if (index < 232) {
            const auto n = index - 16;
            const auto r = n / 36;
            const auto g = (n % 36) / 6;
            const auto b = n % 6;
            return pack_rgb(
                static_cast<uint8_t>(r * 256 / 6),
                static_cast<uint8_t>(g * 256 / 6),
                static_cast<uint8_t>(b * 256 / 6));
        }

        if (index < 256) {
            const auto level = static_cast<uint8_t>((index - 232) * 256 / 24);
            return pack_rgb(level, level, level);
        }

        return 0;
    }

} // anonymous namespace


namespace nx::tui {

// ── Accessors ─────────────────────────────────────────────────────────────────

uint8_t color::r() const noexcept
{
    switch (type_) {
    case color_type::true_color:
        return unpack(value_, g_true_red_shift);
    case color_type::palette256:
    case color_type::palette16:
        return unpack(palette256_to_rgb(value_), g_true_red_shift);
    default:
        return 0;
    }
}

uint8_t color::g() const noexcept
{
    switch (type_) {
    case color_type::true_color:
        return unpack(value_, g_true_green_shift);
    case color_type::palette256:
    case color_type::palette16:
        return unpack(palette256_to_rgb(value_), g_true_green_shift);
    default:
        return 0;
    }
}

uint8_t color::b() const noexcept
{
    switch (type_) {
    case color_type::true_color:
        return unpack(value_, g_true_blue_shift);
    case color_type::palette256:
    case color_type::palette16:
        return unpack(palette256_to_rgb(value_), g_true_blue_shift);
    default:
        return 0;
    }
}

uint8_t color::a() const noexcept
{
    if (type_ == color_type::true_color)
        return unpack(value_, g_true_alpha_shift);
    return 255;
}

// ── ANSI output ───────────────────────────────────────────────────────────────

std::string color::to_ansi(bool background) const
{
    switch (type_) {
    case color_type::palette1:
        return "\x1b[0m";
    case color_type::palette16:
    case color_type::palette256:
        if (background)
            return fmt::format("\x1b[48;5;{}m", value_);
        return fmt::format("\x1b[38;5;{}m", value_);
    case color_type::true_color:
        return fmt::format("\x1b[{};2;{};{};{}m",
                           background ? 48 : 38, r(), g(), b());
    default:
        return {};
    }
}

// ── Debug name ────────────────────────────────────────────────────────────────

std::string color::name() const
{
    switch (type_) {
    case color_type::palette1:
        return "palette1::default";
    case color_type::palette16:
        return fmt::format("palette16::{}", value_);
    case color_type::palette256: {
        if (value_ < 16)  return fmt::format("palette256::{}", value_);
        if (value_ < 232) {
            const auto n = value_ - 16;
            return fmt::format("palette256::rgb({},{},{})", n/36, (n%36)/6, n%6);
        }
        if (value_ < 256) return fmt::format("palette256::gray({})", value_ - 232);
        return "palette256::invalid";
    }
    case color_type::true_color:
        return fmt::format("rgba::#{:08x}", value_);
    default:
        return "invalid";
    }
}

// ── Named constants ───────────────────────────────────────────────────────────

const color color::default_color   { color_type::palette1,  0  };
const color color::black           {
    terminal::get_color_support() == color_type::true_color ?
    rgb(1, 1, 1) : color(color_type::palette16, 0)  };
const color color::red             { color_type::palette16, 1  };
const color color::green           { color_type::palette16, 2  };
const color color::yellow          { color_type::palette16, 3  };
const color color::blue            { color_type::palette16, 4  };
const color color::magenta         { color_type::palette16, 5  };
const color color::cyan            { color_type::palette16, 6  };
const color color::gray_light      { color_type::palette16, 7  };
const color color::gray_dark       { color_type::palette16, 8  };
const color color::red_bright      { color_type::palette16, 9  };
const color color::green_bright    { color_type::palette16, 10 };
const color color::yellow_bright   { color_type::palette16, 11 };
const color color::blue_bright     { color_type::palette16, 12 };
const color color::magenta_bright  { color_type::palette16, 13 };
const color color::cyan_bright     { color_type::palette16, 14 };
const color color::white           { color_type::palette16, 15 };

// ── Factory methods ───────────────────────────────────────────────────────────

color color::rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return color(r, g, b);
}

color color::rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return color(r, g, b, a);
}

color color::from_palette16(uint32_t index)
{
    return color(color_type::palette16, index);
}

color color::from_palette256(uint32_t index)
{
    return color(color_type::palette256, index);
}

color color::interpolate(float t, const color & a, const color & b)
{
    auto interp = [t](uint8_t a_u, uint8_t b_u) -> uint8_t {
        constexpr float gamma = 2.2f;
        const float af = pow_f(static_cast<float>(a_u), gamma);
        const float bf = pow_f(static_cast<float>(b_u), gamma);
        const float cf = af * (1.0f - t) + bf * t;
        return static_cast<uint8_t>(pow_f(cf, 1.0f / gamma));
    };
    return rgb(interp(a.r(), b.r()), interp(a.g(), b.g()), interp(a.b(), b.b()));
}

color color::scale(const color & c, float factor)
{
    const float f = factor < 0.f ? 0.f : (factor > 1.f ? 1.f : factor);
    return rgb(
        static_cast<uint8_t>(c.r() * f),
        static_cast<uint8_t>(c.g() * f),
        static_cast<uint8_t>(c.b() * f));
}

color color::blend(const color & a, const color & b)
{
    return interpolate(a.a() / 255.0f, a, b);
}

color color::negative(const color & c)
{
    return rgba(255 - c.r(), 255 - c.g(), 255 - c.b(), c.a());
}

// ── Constructors ──────────────────────────────────────────────────────────────

color::color()
    : color(color_type::palette1, 0)
{}

color::color(color_type type, uint32_t value)
    : type_  { type  }
    , value_ { value }
{}

color::color(uint8_t r, uint8_t g, uint8_t b)
    : type_ { terminal::get_color_support() == color_type::true_color
              ? color_type::true_color : color_type::palette256 }
    , value_ { type_ == color_type::true_color
               ? pack_rgb(r, g, b) : rgb_to_palette256(r, g, b) }
{}

color::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : type_ { terminal::get_color_support() == color_type::true_color
              ? color_type::true_color : color_type::palette256 }
    , value_ { type_ == color_type::true_color
               ? pack_rgba(r, g, b, a) : rgb_to_palette256(r, g, b) }
{}

} // namespace nx::tui

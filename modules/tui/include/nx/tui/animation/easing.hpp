#pragma once

#include <cmath>

namespace nx::tui {

using easing_fn = float(*)(float t);

namespace easing {

inline float linear(float t) noexcept { return t; }

inline float ease_in(float t) noexcept { return t * t; }

inline float ease_out(float t) noexcept { return t * (2.0f - t); }

inline float ease_in_out(float t) noexcept
{
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

inline float ease_in_cubic(float t) noexcept { return t * t * t; }

inline float ease_out_cubic(float t) noexcept
{
    const float s = t - 1.0f;
    return s * s * s + 1.0f;
}

inline float ease_in_out_cubic(float t) noexcept
{
    if (t < 0.5f) return 4.0f * t * t * t;
    const float s = 2.0f * t - 2.0f;
    return 0.5f * s * s * s + 1.0f;
}

inline float bounce_out(float t) noexcept
{
    if (t < 1.0f / 2.75f)       return 7.5625f * t * t;
    if (t < 2.0f / 2.75f) { t -= 1.500f / 2.75f; return 7.5625f * t * t + 0.75f;    }
    if (t < 2.5f / 2.75f) { t -= 2.250f / 2.75f; return 7.5625f * t * t + 0.9375f;  }
    t -= 2.625f / 2.75f;        return 7.5625f * t * t + 0.984375f;
}

inline float elastic_out(float t) noexcept
{
    if (t <= 0.0f || t >= 1.0f) return t;
    constexpr float pi = 3.14159265f;
    return std::pow(2.0f, -10.0f * t)
         * std::sin((t - 0.075f) * (2.0f * pi) / 0.3f)
         + 1.0f;
}

} // namespace easing
} // namespace nx::tui

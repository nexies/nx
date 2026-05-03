#pragma once

#include <nx/tui/types/point.hpp>
#include <nx/tui/types/size.hpp>

#include <cstdlib>

namespace nx::tui {
    template<typename T = int>
    class rect {
    public:
        using value_type = typename size_traits<T>::value_type;
        using point_type = point<T>;
        using size_type = size<T>;

    private:
        point_type pos_{};
        size_type size_{};

    public:
        constexpr
        rect() noexcept = default;

        explicit constexpr
        rect(size_type s) noexcept
            : pos_{}, size_{s} {
        }

        constexpr
        rect(point_type top_left, size_type s) noexcept
            : pos_{top_left}, size_{s} {
        }

        constexpr
        rect(point_type top_left, point_type bottom_right) noexcept
            : pos_{top_left}
              , size_{bottom_right.x - top_left.x, bottom_right.y - top_left.y} {
        }

        constexpr
        rect(value_type x, value_type y, value_type w, value_type h) noexcept
            : pos_{x, y}, size_{h, w} {
        }

        // ── Accessors ─────────────────────────────────────────────────────────

        NX_NODISCARD constexpr value_type
        x() const noexcept { return pos_.x; }

        NX_NODISCARD constexpr value_type
        y() const noexcept { return pos_.y; }

        NX_NODISCARD constexpr value_type
        width() const noexcept { return size_.width; }

        NX_NODISCARD constexpr value_type
        height() const noexcept { return size_.height; }

        NX_NODISCARD constexpr size_type
        size() const noexcept { return size_; }

        NX_NODISCARD constexpr point_type
        top_left() const noexcept { return pos_; }

        NX_NODISCARD constexpr point_type
        top_right() const noexcept { return {pos_.x + size_.width - 1, pos_.y}; }

        NX_NODISCARD constexpr point_type
        bottom_left() const noexcept { return {pos_.x, pos_.y + size_.height - 1}; }

        NX_NODISCARD constexpr point_type
        bottom_right() const noexcept { return {pos_.x + size_.width - 1, pos_.y + size_.height - 1}; }

        // ── Queries ───────────────────────────────────────────────────────────

        NX_NODISCARD constexpr bool
        operator==(const rect &o) const noexcept { return pos_ == o.pos_ && size_ == o.size_; }

        NX_NODISCARD constexpr bool
        operator!=(const rect &o) const noexcept { return !(*this == o); }

        NX_NODISCARD constexpr bool
        contains(value_type px, value_type py) const noexcept {
            return px >= pos_.x && px <= pos_.x + size_.width
                   && py >= pos_.y && py <= pos_.y + size_.height;
        }

        NX_NODISCARD constexpr bool
        contains(const point_type &p) const noexcept { return contains(p.x, p.y); }

        NX_NODISCARD constexpr value_type
        area() const noexcept { return std::abs(size_.width * size_.height); }

        NX_NODISCARD constexpr value_type
        perimeter() const noexcept { return std::abs(2 * (size_.width + size_.height)); }

        // ── Mutation ──────────────────────────────────────────────────────────

        NX_NODISCARD constexpr bool
        empty() const noexcept { return size_.width <= 0 || size_.height <= 0; }

        // Returns the intersection of this rect and other.
        // Returns an empty rect if they don't overlap.
        NX_NODISCARD constexpr rect
        intersect(const rect &other) const noexcept {
            const value_type x1 = std::max(x(), other.x());
            const value_type y1 = std::max(y(), other.y());
            const value_type x2 = std::min(x() + width(), other.x() + other.width());
            const value_type y2 = std::min(y() + height(), other.y() + other.height());
            if (x2 <= x1 || y2 <= y1) return {};
            return {x1, y1, x2 - x1, y2 - y1};
        }

        NX_NODISCARD rect
        translated(value_type dx, value_type dy) const noexcept {
            return {pos_.x + dx, pos_.y + dy, size_.width, size_.height};
        }

        rect &
        translate(value_type dx, value_type dy) noexcept { return *this = translated(dx, dy); }
    };
} // namespace nx::tui

//
// Created by nexie on 17.03.2026.
//

#ifndef NX_TUI_RECT_HPP
#define NX_TUI_RECT_HPP

#include <type_traits>
#include <nx/tui/types/size.hpp>

#include "point.hpp"

namespace nx::tui
{
    template<typename Type>
    class Rect
    {
    public:
        using traits_type = SizeTraits<Type>;
        using units = traits_type::size_type;

        using point_type = Point<units>;
        using size_type = Size<units>;

        using rect_type = Rect<units>;

    private:
        point_type pos_;
        size_type size_;

        public:

        explicit
        Rect(size_type size = {}) noexcept
            : pos_ {0, 0}
            , size_ {size}
        { }

        Rect(point_type topLeft, size_type size) noexcept
            : pos_ {topLeft}
            , size_ {size}
        { }

        Rect(point_type topLeft, point_type bottomRight) noexcept
            : pos_ { topLeft }
            , size_ { bottomRight.x - topLeft.x , bottomRight.y - topLeft.y }
        { }

        Rect(units x, units y, units width, units height) noexcept
            : pos_ { x, y }
            , size_ { width, height }
        { }

        [[nodiscard]] constexpr units
        x () const noexcept
        {
            return pos_.x;
        }

        [[nodiscard]] constexpr units
        y () const noexcept
        {
            return pos_.y;
        }

        [[nodiscard]] constexpr units
        height () const noexcept
        {
            return size_.height;
        }

        [[nodiscard]] constexpr units
        width () const noexcept
        {
            return size_.width;
        }

        [[nodiscard]] constexpr point_type
        topLeft () const noexcept
        {
            return pos_;
        }

        [[nodiscard]] constexpr point_type
        topRight () const noexcept
        {
            return { pos_.x + size_.width - 1, pos_.y };
        }

        [[nodiscard]] constexpr point_type
        bottomLeft () const noexcept
        {
            return { pos_.x, pos_.y + size_.height - 1 };
        }

        [[nodiscard]] constexpr point_type
        bottomRight () const noexcept
        {
            return { pos_.x + size_.width - 1, pos_.y + size_.height - 1 };
        }

        [[nodiscard]] constexpr bool
        contains (units x, units y) const noexcept
        {
            return (x >= pos_.x && x <= pos_.x + size_.width)
                &&  ( y >= pos_.y && y <= pos_.y + size_.height);
        }

        [[nodiscard]] constexpr bool
        contains (const point_type & point) const noexcept
        {
            return contains(point.x, point.y);
        }

        [[nodiscard]] constexpr bool
        contains (const Rect & other)
        {
            return contains(other.topLeft(), other.topRight());
        }

        [[nodiscard]] rect_type
        translated (units x, units y) noexcept
        {
            return rect_type {pos_.x + x, pos_.y + y, size_.width, size_.height};
        }

        rect_type &
        translate (units x, units y) noexcept
        {
            return *this = translated (x, y);
        }

        [[nodiscard]] constexpr units
        field () const noexcept
        {
            return std::abs(size_.width * size_.height);
        }

        [[nodiscard]] constexpr units
        perimeter () const noexcept
        {
            return std::abs(2*(size_.width + size_.height));
        }

        [[nodiscard]] constexpr size_type
        size () const noexcept
        {
            return size_;
        }
    };
}

#endif //NX_TUI_RECT_HPP
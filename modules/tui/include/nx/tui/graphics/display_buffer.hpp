#pragma once

#include <nx/tui/graphics/pixel.hpp>
#include <nx/tui/types/rect.hpp>

#include <vector>

namespace nx::tui {

    class painter;

    class display_buffer
    {
        friend class painter;

    public:
        using pixel_type             = pixel;
        using pixel_reference        = pixel_type &;
        using const_pixel_reference  = const pixel_type &;
        using units                  = int;
        using character_type         = pixel_type::character_type;
        using rect_type              = rect<units>;
        using size_type              = size<units>;

    private:
        rect_type              rect_;
        std::vector<pixel_type> pixels_;

    public:
        display_buffer(units rows, units cols);
        explicit display_buffer(size_type s);
        virtual ~display_buffer();

        [[nodiscard]] painter get_painter();

        void resize(units rows, units cols);

        [[nodiscard]] pixel_reference       pixel_at(units x, units y);
        [[nodiscard]] const_pixel_reference pixel_at(units x, units y) const;

        [[nodiscard]] character_type &       at(units x, units y);
        [[nodiscard]] const character_type & at(units x, units y) const;

        [[nodiscard]] units rows() const noexcept { return rect_.height(); }
        [[nodiscard]] units cols() const noexcept { return rect_.width();  }

        void clear();

        [[nodiscard]] size_type size() const noexcept { return rect_.size(); }
        [[nodiscard]] rect_type rect() const noexcept { return rect_; }

        [[nodiscard]] const std::vector<pixel_type> & data() const noexcept { return pixels_; }
    };

} // namespace nx::tui

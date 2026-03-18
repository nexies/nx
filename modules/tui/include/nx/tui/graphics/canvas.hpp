//
// Created by nexie on 17.03.2026.
//

#ifndef NX_TUI_CANVAS_HPP
#define NX_TUI_CANVAS_HPP

#include <nx/tui/graphics/pixel.hpp>
#include <nx/tui/types/rect.hpp>

#include <vector>

namespace nx::tui
{
    class Canvas
    {
    public:
        using pixel_type = Pixel;
        using pixel_reference = pixel_type&;
        using const_pixel_reference = const pixel_type&;
        using size_type = std::size_t;
        using character_type = pixel_type::character_type;

    private:
        Rect<size_type> rect_;
        std::vector<pixel_type> pixels_;
    public:
        Canvas(size_type hrow, size_type wcol);

        virtual
        ~Canvas();

        void
        resize (size_type hrow, size_type wcol);

        pixel_reference
        pixelAt(size_type x, size_type y);

        [[nodiscard]] const_pixel_reference
        pixelAt(size_type x, size_type y) const;

        [[nodiscard]]
        character_type&
        at(size_type x, size_type y);

        [[nodiscard]] const character_type&
        at(size_type x, size_type y) const;

        [[nodiscard]] size_type
        hrow () const;

        [[nodiscard]] size_type
        wcol () const;

        void
        clear ();

    };
}

#endif //NX_TUI_CANVAS_HPP
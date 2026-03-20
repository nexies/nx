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
    class Painter;

    class DisplayBuffer
    {

        friend class Painter;

    public:
        using pixel_type = Pixel;
        using pixel_reference = pixel_type&;
        using const_pixel_reference = const pixel_type&;
        using units = int;
        using character_type = pixel_type::character_type;


        using rect_type = Rect<units>;
        using size_type = Size<units>;
    private:
        rect_type rect_;
        std::vector<pixel_type> pixels_;
    public:
        DisplayBuffer(units hrow, units wcol);
        DisplayBuffer(Size<units> size);

        virtual
        ~DisplayBuffer();

        [[nodiscard]] Painter
        getPainter ();

        void
        resize (units hrow, units wcol);

        pixel_reference
        pixelAt (units x, units y);

        [[nodiscard]] const_pixel_reference
        pixelAt (units x, units y) const;

        [[nodiscard]]
        character_type&
        at (units x, units y);

        [[nodiscard]] const character_type&
        at (units x, units y) const;

        [[nodiscard]] units
        hrow () const;

        [[nodiscard]] units
        wcol () const;

        void
        clear ();

        [[nodiscard]] size_type
        size () const;

        [[nodiscard]] rect_type
        rect () const;

        [[nodiscard]] const std::vector<pixel_type> &
        data () const;
    };
}

#endif //NX_TUI_CANVAS_HPP
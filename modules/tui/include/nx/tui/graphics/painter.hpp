//
// Created by nexie on 18.03.2026.
//

#ifndef NX_TUI_PAINTER_HPP
#define NX_TUI_PAINTER_HPP

#include <nx/tui/graphics/display_buffer.hpp>

namespace nx::tui {

    class Painter {

    public:
        using rect_type = Rect<int>;
        using size_type = Size<int>;
        using point_type = Point<int>;
        using buffer_type = DisplayBuffer;

    private:
        buffer_type & buffer_;
        rect_type rect_;

        Color color_;
        Color background_color_;
        PixelStyle pixel_style_;

        [[nodiscard]] point_type
        _projectPoint (const point_type & pos) const;

    public:
        explicit
        Painter (buffer_type & buffer);
        Painter (buffer_type & buffer, rect_type rect);

        void
        enableStyle (const PixelStyle & pixel_style);

        void
        disableStyle (const PixelStyle & pixel_style);

        void
        setStyle (const PixelStyle & pixel_style);

        void
        setColor (const Color & color);

        void
        setBackgroundColor (const Color & color);

        void
        drawText (const point_type & pos, const std::string & text) const;
    };
}

#endif //NX_TUI_PAINTER_HPP

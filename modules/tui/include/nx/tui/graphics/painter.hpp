#pragma once

#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/types/style_option.hpp>

#include <string>

namespace nx::tui {

    class painter {
    public:
        using rect_type   = rect<int>;
        using size_type   = size<int>;
        using point_type  = point<int>;
        using buffer_type = display_buffer;

    private:
        buffer_type & buffer_;
        rect_type     rect_;
        color         color_;
        color         background_color_;
        pixel_style   pixel_style_;

        [[nodiscard]] point_type _project_point(const point_type & pos) const;

    public:
        explicit painter(buffer_type & buffer);
        painter(buffer_type & buffer, rect_type clip_rect);

        void enable_style(pixel_style style);
        void disable_style(pixel_style style);
        void set_style(pixel_style style);

        void set_color(const color & c);
        void set_background_color(const color & c);

        void draw_text(const point_type & pos, const std::string & text) const;
        void draw_char(const point_type & pos, const std::string & ch) const;

        // Fill the entire clip rect with ch using the current color/style.
        void fill(const std::string & ch = " ") const;

        // Apply a style_option: sets only the fields that are present.
        void apply_style(const style_option & s) noexcept;
    };

} // namespace nx::tui

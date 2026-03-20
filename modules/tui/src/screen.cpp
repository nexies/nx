//
// Created by nexie on 20.03.2026.
//

#include <nx/tui/terminal/Screen.hpp>
#include <nx/tui/terminal/terminal.hpp>
#include <nx/tui/App.hpp>
#include <nx/tui/types/color.hpp>

namespace nx::tui
{
    Screen::Screen(Object* parent) :
        Object(parent),
        buffer_(Terminal::GetWindowSize().chars)
    {
        ::nx::connect(nxApp, &Application::windowChanged,
            this, &Screen::onWindowSizeChanged);

        ::nx::connect(this, &Screen::_signalToRender,
            this, &Screen::_renderOnScreen, Connection::Queued);
    }

    void Screen::render()
    {
        // _renderOnScreen();
        NX_EMIT(_signalToRender)
    }

    DisplayBuffer& Screen::getBuffer()
    {
        return buffer_;
    }

    void Screen::onWindowSizeChanged(WindowSize w)
    {
        buffer_.resize(w.chars.height, w.chars.width);
    }

    void Screen::_renderOnScreen()
    {
        const auto rect = buffer_.rect();
        const auto size = rect.size();

        const auto & data = buffer_.data();

        thread_local fmt::memory_buffer g_fmt_buffer;

        g_fmt_buffer.clear();
        PixelStyle  current_style = PixelStyleOptions::None;
        PixelStyle  delta_style = current_style;

        Color current_color = Color::Default;
        Color current_background = Color::Default;
        int counter = 0;

        Terminal::MoveCursorHome();

        for (auto & pixel: data)
        {

            delta_style = current_style ^ pixel.style;

            if (delta_style)
            {
                Terminal::ResetPixelStyle( delta_style & ~(pixel.style) );
                Terminal::SetPixelStyle( delta_style & pixel.style );
            }

            current_style = pixel.style;

            if (pixel.foreground_color != current_color)
            {
                Terminal::SetColor(pixel.foreground_color);
                current_color = pixel.foreground_color;
            }

            if (pixel.background_color != current_background)
            {
                Terminal::SetBackgroundColor(pixel.background_color);
                current_background = pixel.background_color;
            }

            Terminal::Print(pixel.character);
            if ((counter % rect.width() - 1) == 0)
            {
                Terminal::MoveCursorNextLineBegin(0);
            }

            counter += 1;
        }
    }
}

//
// Created by nexie on 17.03.2026.
//

#ifndef NX_SCREEN_HPP
#define NX_SCREEN_HPP

#include <nx/core/Object.hpp>
#include <nx/tui/graphics/display_buffer.hpp>

namespace nx::tui
{
    class Screen : public Object
    {
        DisplayBuffer buffer_;
    public:
        explicit
        Screen(Object * parent = nullptr);
        void render ();


        DisplayBuffer & getBuffer();
    protected:
        void onWindowSizeChanged(WindowSize w);

        NX_SIGNAL(_signalToRender)
        void _renderOnScreen ();
    };
}

#endif //NX_SCREEN_HPP
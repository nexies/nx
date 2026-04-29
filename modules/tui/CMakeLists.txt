add_library(nx_tui
    src/App.cpp
    src/terminal.cpp
    src/animation/animator.cpp
    src/animation/animation_manager.cpp
    src/types/color.cpp
    src/graphics/painter.cpp
    src/graphics/display_buffer.cpp
    src/input/escape_parser.cpp
    src/input/input_reader.cpp
    src/widgets/widget.cpp
    src/widgets/screen.cpp
    src/widgets/label.cpp
    src/widgets/button.cpp
    src/widgets/line_edit.cpp
    src/widgets/frame.cpp
    src/widgets/box.cpp
    src/widgets/scroll_area.cpp

    include/nx/tui/application.hpp
    include/nx/tui/terminal/terminal.hpp
    include/nx/tui/terminal/guards/alt_screen_guard.hpp
    include/nx/tui/terminal/guards/cursor_guard.hpp
    include/nx/tui/terminal/guards/mouse_guard.hpp
    include/nx/tui/terminal/guards/raw_mode_guard.hpp
    include/nx/tui/graphics/display_buffer.hpp
    include/nx/tui/graphics/painter.hpp
    include/nx/tui/graphics/pixel.hpp
    include/nx/tui/types/color.hpp
    include/nx/tui/types/point.hpp
    include/nx/tui/types/rect.hpp
    include/nx/tui/types/size.hpp
    include/nx/tui/input/key.hpp
    include/nx/tui/input/key_event.hpp
    include/nx/tui/input/mouse_event.hpp
    include/nx/tui/input/input_event.hpp
    include/nx/tui/input/escape_parser.hpp
    include/nx/tui/input/input_reader.hpp
    include/nx/tui/input/event_filter.hpp
    include/nx/tui/types/size_policy.hpp
    include/nx/tui/types/style_option.hpp
    include/nx/tui/types/style_modifier.hpp
    include/nx/tui/types/text_align.hpp
    include/nx/tui/widgets/widget.hpp
    include/nx/tui/widgets/screen.hpp
    include/nx/tui/widgets/label.hpp
    include/nx/tui/widgets/button.hpp
    include/nx/tui/widgets/line_edit.hpp
    include/nx/tui/widgets/frame.hpp
    include/nx/tui/widgets/box.hpp
    include/nx/tui/widgets/scroll_area.hpp
    include/nx/tui/types/border_style.hpp
    include/nx/tui/types/margin.hpp
    include/nx/tui/terminal/guards.hpp
    include/nx/tui/terminal.hpp
)

target_link_libraries(nx_tui PUBLIC
    nx::core2
)

nx_configure_component(nx_tui
    NAME tui
    VERSION 0.0.2
)

if (NX_BUILD_EXAMPLES)
    add_subdirectory(demo)
endif()

if (NX_BUILD_TESTS)
    add_subdirectory(tests)
endif()

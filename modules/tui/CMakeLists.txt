add_library(nx_tui
    src/App.cpp
        include/nx/tui/terminal/terminal.hpp
        src/terminal.cpp
        include/nx/tui/types/size.hpp
        include/nx/tui/terminal/Screen.hpp
        include/nx/tui/graphics/pixel.hpp
        include/nx/tui/types/rect.hpp
        include/nx/tui/types/point.hpp
        include/nx/tui/types/color.hpp
        src/types/color.cpp
        include/nx/tui/graphics/painter.hpp
        src/ansi_codes.p.hpp
        src/screen.cpp
        src/graphics/painter.cpp
        src/graphics/display_buffer.cpp
)

target_link_libraries(nx_tui PUBLIC
        nx::core
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_tui
        NAME tui
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        VERSION 0.0.1
)
add_library(nx_tui
    src/App.cpp
        include/nx/tui/terminal/Terminal.hpp
        src/Terminal.cpp
        include/nx/tui/types/Size.hpp
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
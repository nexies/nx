add_library(nx_tui
    src/App.cpp
        include/nx/tui/Terminal.hpp
        src/Terminal.cpp
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
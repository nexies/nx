add_library(nx_app
        src/App.cpp
        src/nxapp.cpp
)

include(nxMakeModule)

target_link_libraries(nx_app PUBLIC
        nx::core
)

nx_make_module(
        TARGET nx_app
        NAME app
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
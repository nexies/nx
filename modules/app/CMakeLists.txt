add_library(app
        main.cpp
)

include(nxMakeModule)
nx_make_module(
        TARGET app
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
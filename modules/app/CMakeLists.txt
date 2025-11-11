add_library(nx_app
        main.cpp
)

include(nxMakeModule)
nx_make_module(
        TARGET nx_app
        NAME app
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
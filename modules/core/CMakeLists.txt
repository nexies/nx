add_library(core
        src/core.cpp
)

include(nxMakeModule)
nx_make_module(
        TARGET core
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)


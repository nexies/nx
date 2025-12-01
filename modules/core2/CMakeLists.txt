add_library(nx_core2
        src/core2.cpp
        include/nx/core2/Thread.hpp
)

include(nxMakeModule)

nx_make_module(
        NAME core2
        TARGET nx_core2
        HEADERS_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
)
add_library(nx_macro
        include/nx/macro.hpp
        macro.cpp
        include/nx/macro/overload.hpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_macro
        NAME macro
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
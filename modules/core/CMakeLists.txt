add_library(nx_core
        src/core.cpp
        include/nx/core/Callable.hpp
        include/nx/core/Tuple.hpp

)

include(nxMakeModule)
nx_make_module(
        TARGET nx_core
        NAME core
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)


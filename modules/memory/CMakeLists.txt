add_library(nx_memory
        src/DataPool.cpp
        include/nx/memory/data_pool.hpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_memory
        NAME memory
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
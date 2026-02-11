add_library(nx_io
    src/io.cpp
        include/nx/io/IOStream.hpp
        include/nx/io/Socket.hpp
)

target_link_libraries(nx_io PRIVATE
        nx::core
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_io
        NAME io
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        VERSION 0.0.1
)
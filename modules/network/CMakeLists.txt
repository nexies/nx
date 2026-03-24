add_library(nx_network
        src/network.cpp
        include/nx/network/UdpSocket.hpp
        src/UdpSocket.cpp
)

target_link_libraries(nx_network PRIVATE
        nx_core
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_network
        NAME network
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        VERSION 0.0.1
)
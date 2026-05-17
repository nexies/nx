set(NX_NETWORK_SOURCES
    src/network.cpp
    src/types.cpp
    src/socket.cpp
    src/buffer/read_buffer.cpp
    src/buffer/read_accumulator.cpp
    src/framing/length_prefix_framer.cpp
    src/framing/delimiter_framer.cpp
    src/resolver.cpp
    src/udp/socket.cpp
    src/tcp/socket.cpp
    src/tcp/server.cpp
    src/tcp/reconnector.cpp
    src/http/request.cpp
    src/http/response.cpp
    src/http/request_parser.cpp
    src/http/connection.cpp
    src/http/server.cpp
)

if(WIN32)
    list(APPEND NX_NETWORK_SOURCES src/detail/socket_impl_win.cpp)
else()
    list(APPEND NX_NETWORK_SOURCES src/detail/socket_impl_posix.cpp)
endif()

add_library(nx_network ${NX_NETWORK_SOURCES})

target_include_directories(nx_network PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(nx_network PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(nx_network PUBLIC nx::core2)

if(WIN32)
    target_link_libraries(nx_network PRIVATE ws2_32)
endif()

nx_configure_component(nx_network
    NAME network
    VERSION 0.6.0
)

add_subdirectory(demo)
add_subdirectory(tests)

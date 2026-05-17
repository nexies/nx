include(nxConfigureTest)

nx_configure_test(buffer
    COMPONENT network
    SOURCE test_buffer.cpp
)

nx_configure_test(buffered_socket
    COMPONENT network
    SOURCE test_buffered_socket.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_test(framing
    COMPONENT network
    SOURCE test_framing.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_test(http_unit
    COMPONENT network
    SOURCE test_http_unit.cpp
)

nx_configure_test(http
    COMPONENT network
    SOURCE test_http.cpp
    LINK_PUBLIC nx_logging
)

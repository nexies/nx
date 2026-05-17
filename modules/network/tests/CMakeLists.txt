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

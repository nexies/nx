include(nxConfigureDemo)

nx_configure_demo(tcp_echo
    COMPONENT network
    SOURCE tcp_echo_demo.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_demo(udp_echo
    COMPONENT network
    SOURCE udp_echo_demo.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_demo(resolver
    COMPONENT network
    SOURCE resolver_demo.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_demo(buffered_echo
    COMPONENT network
    SOURCE buffered_echo_demo.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_demo(framing
    COMPONENT network
    SOURCE framing_demo.cpp
    LINK_PUBLIC nx_logging
)

nx_configure_demo(http_server
    COMPONENT network
    SOURCE http_server_demo.cpp
    LINK_PUBLIC nx_logging
)

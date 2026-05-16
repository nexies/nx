include(nxConfigureDemo)

nx_configure_demo(tcp_echo
    COMPONENT network
    SOURCE tcp_echo_demo.cpp
)

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

if(TARGET nx_pcap)
    nx_configure_test(pcap_reader
        COMPONENT pcap
        SOURCE test_pcap_reader.cpp
    )

    nx_configure_test(pcap_device
        COMPONENT pcap
        SOURCE test_pcap_device.cpp
    )

    nx_configure_test(pcap_replayer
        COMPONENT pcap
        SOURCE test_pcap_replayer.cpp
    )
endif()

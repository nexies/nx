add_executable(nx_example_udp
        main.cpp
)

target_link_libraries(nx_example_udp PUBLIC
        nx::core
        nx::network
)
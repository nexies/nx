add_executable(nx_connection_example
        main.cpp
)


target_link_libraries(nx_connection_example PUBLIC
        nx::core
)


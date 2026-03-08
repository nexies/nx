add_executable(nx_example_connection
        main.cpp
)


target_link_libraries(nx_example_connection PUBLIC
        nx::core
)


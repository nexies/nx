add_executable(nx_example_tui
        main.cpp
)


target_link_libraries(nx_example_tui PUBLIC
        nx::tui
)

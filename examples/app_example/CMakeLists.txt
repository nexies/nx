add_executable(nx_example_app
        main.cpp
)

target_link_libraries(nx_example_app PRIVATE
        nx::core
)
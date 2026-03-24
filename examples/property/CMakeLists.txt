add_executable(nx_example_property
        main.cpp
)

target_link_libraries(nx_example_property PRIVATE
        nx::core
)
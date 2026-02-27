add_executable(nx_app_example
        main.cpp
)

target_link_libraries(nx_app_example PRIVATE
        nx::core
)
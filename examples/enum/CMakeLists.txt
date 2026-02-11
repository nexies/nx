add_executable(nx_enum_example main.cpp)

target_link_libraries(nx_enum_example PRIVATE
        nx::core
)
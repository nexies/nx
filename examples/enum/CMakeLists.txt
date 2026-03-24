add_executable(nx_example_enum main.cpp)

target_link_libraries(nx_example_enum PRIVATE
        nx::core
)
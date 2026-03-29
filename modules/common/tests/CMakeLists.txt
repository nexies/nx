add_executable(nx_common_test_source_location
        nx_common_test_source_location.cpp
)

target_link_libraries(nx_common_test_source_location
        PUBLIC
        nx::common
)
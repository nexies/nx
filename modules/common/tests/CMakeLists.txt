add_executable(nx_common_test_source_location
        nx_common_test_source_location.cpp
)

target_link_libraries(nx_common_test_source_location
        PUBLIC
        nx::common
)

add_executable(nx_common_test_error
        nx_common_test_error.cpp
)

target_link_libraries(nx_common_test_error
        PRIVATE
        nx::common
)

add_executable(nx_common_test_result
        nx_common_test_result.cpp
)

target_link_libraries(nx_common_test_result
        PRIVATE
        nx::common
)


# nx_configure_test(result
#       COMPONENT common core
#       SOURCES nx_common_test_result.cpp
#)

include(nxConfigureTest)

nx_configure_test(source_location
        COMPONENT common
        SOURCE test_source_location.cpp
)

nx_configure_test(error
        COMPONENT common
        SOURCE test_error.cpp
)

nx_configure_test(result
        COMPONENT common
        SOURCE test_result.cpp
)

nx_configure_test(utf8
        COMPONENT common
        SOURCE test_utf8.cpp
)

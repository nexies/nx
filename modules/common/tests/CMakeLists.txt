include(nxConfigureTest)

nx_configure_test(source_location
        COMPONENT common
)

nx_configure_test(error
        COMPONENT common
        BINARY_NAME nx_common_test_error_2
)

nx_configure_test(result
        COMPONENT common
)

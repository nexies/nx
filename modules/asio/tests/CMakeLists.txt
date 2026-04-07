include(nxConfigureTest)

nx_configure_test(backend
        COMPONENT asio
        SOURCE backend_test.cpp
        NO_CATCH
)

nx_configure_test(io_context
        COMPONENT asio
        SOURCE test_io_context.cpp
)
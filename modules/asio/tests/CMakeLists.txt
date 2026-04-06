include(nxConfigureTest)

nx_configure_test(backend
        COMPONENT asio
        SOURCES backend_test.cpp
)

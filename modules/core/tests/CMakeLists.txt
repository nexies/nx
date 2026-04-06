include(nxConfigureTest)

nx_configure_test(context
        COMPONENT core
        SOURCES asio_context.cpp
)

nx_configure_test(handle_notifier
        COMPONENT core
        SOURCES handle_notifier.cpp
)

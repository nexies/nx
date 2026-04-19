include(nxConfigureTest)

#nx_configure_test(context
#        COMPONENT core
#        SOURCE asio_context.cpp
#        NO_CATCH
#)

nx_configure_test(handle_notifier
        COMPONENT core
        SOURCE handle_notifier.cpp
        NO_CATCH
)

nx_configure_test(manual_meta_object
        COMPONENT core
        SOURCE try_meta_object.cpp
        NO_CATCH
)
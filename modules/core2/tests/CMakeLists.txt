include(nxConfigureTest)

nx_configure_test(signals
        COMPONENT core2
        SOURCE test_signals.cpp
)

nx_configure_test(object_tree
        COMPONENT core2
        SOURCE test_object_tree.cpp
)

nx_configure_test(connections
        COMPONENT core2
       SOURCE test_connections.cpp
)

nx_configure_test(manual_property
        COMPONENT core2
        SOURCE manual_test_property.cpp
        NO_CATCH
)

nx_configure_test(property
        COMPONENT core2
        SOURCE test_property.cpp
)
include(nxConfigureTest)

nx_configure_test(levels
        COMPONENT logging
        SOURCE test_levels.cpp
)

nx_configure_test(pattern_formatter
        COMPONENT logging
        SOURCE test_pattern_formatter.cpp
)

nx_configure_test(registry
        COMPONENT logging
        SOURCE test_registry.cpp
)

nx_configure_test(sinks
        COMPONENT logging
        SOURCE test_sinks.cpp
)

nx_configure_test(logger_manual
        COMPONENT logging
        SOURCE logger_manual.cpp
        NO_CATCH
)

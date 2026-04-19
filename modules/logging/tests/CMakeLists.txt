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

nx_configure_test(manual
        COMPONENT logging
        SOURCE test_logger_manual.cpp
        NO_CATCH
)

nx_configure_test(manual_macros
        COMPONENT logging
        SOURCE test_logger_manual_macros.cpp
        NO_CATCH
)

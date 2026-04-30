
set(nx_logging_headers
        include/nx/logging/levels.hpp
        include/nx/logging/types.hpp
        include/nx/logging/message.hpp
        include/nx/logging/logger.hpp
        include/nx/logging/registry.hpp
        include/nx/logging/macros.hpp
        include/nx/logging/pattern_formatter.hpp
        include/nx/logging/logging.hpp
        include/nx/logging/sinks/sink.hpp
        include/nx/logging/sinks/stdout_sink.hpp
        include/nx/logging/sinks/basic_file_sink.hpp
        include/nx/logging/sinks/dist_sink.hpp
        include/nx/logging.hpp
)

set(nx_logging_sources
        src/pattern_formatter.cpp
        src/registry.cpp
#        tests/test_logger_manual_macros.cpp
#        tests/test_logger_manual.cpp
)

add_library(nx_logging
        ${nx_logging_headers}
        ${nx_logging_sources}
)

nx_configure_component(nx_logging
        NAME logging
        VERSION 0.1.0
)

target_link_libraries(nx_logging PUBLIC
        nx::common
)

add_subdirectory(tests)

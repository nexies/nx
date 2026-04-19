
set(nx_logging_headers
        include/nx/logging/sinks/basic_sink.hpp
        include/nx/logging/levels.hpp
        include/nx/logging/message.hpp
        include/nx/logging/types.hpp
        include/nx/logging/logger.hpp
        include/nx/logger.hpp

        include/nx/logging/formatters/formatter.hpp
        include/nx/logging/sinks/formatted_sink.hpp
        include/nx/logging/sinks/ansi_console_sink.hpp
        include/nx/logging/formatters/colored_ansi_formatter.hpp
)

set(nx_logging_sources
        src/logger.cpp
        src/message.cpp
        src/formatter.cpp
        include/nx/logging/formatters/flag_formatters.hpp
)

add_library(nx_logging
        ${nx_logging_headers}
        ${nx_logging_sources}
)

nx_configure_component(nx_logging
        NAME logging
        VERSION 0.0.1
)

target_link_libraries(nx_logging PUBLIC
        nx::common
)
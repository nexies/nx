set(nx_core2_headers
    include/nx/core2/detail/function_id.hpp
    include/nx/core2/detail/logger_defs.hpp
    include/nx/core2/detail/signal_defs.hpp
    include/nx/core2/detail/property_defs.hpp
    include/nx/core2/detail/object_defs.hpp
    include/nx/core2/object/connection.hpp
    include/nx/core2/object/connection_info.hpp
    include/nx/core2/object/object.hpp
    include/nx/core2/thread/thread.hpp
    include/nx/core2/loop/loop.hpp
    include/nx/core2/timer/timer.hpp
    include/nx/core2/io_device.hpp
    include/nx/core2/app/application.hpp
    include/nx/core2/core2.hpp
)

set(nx_core2_sources
    src/object/object.cpp
    src/object/connection_info.cpp
    src/thread/thread.cpp
    src/loop/loop.cpp
    src/timer/timer.cpp
    src/app/application.cpp
        include/nx/core2/object/signal_functions.hpp
        include/nx/core2/object.hpp
)

add_library(nx_core2
    ${nx_core2_headers}
    ${nx_core2_sources}
)

target_link_libraries(nx_core2 PUBLIC
    nx::common
    nx::asio
    nx::macro
    nx::logging
)

target_compile_definitions(nx_core2 PUBLIC
    NX_DEVEL_LOGGING=1
    NX_TRACE_SIGNALS=0
)

if (MSVC)
    target_compile_options(nx_core2 PRIVATE /utf-8)
endif()

nx_configure_component(nx_core2
    NAME core2
    VERSION 0.1.0
)

add_subdirectory(tests)

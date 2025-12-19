add_library(nx_core
        src/core.cpp
        src/Connection.cpp
        src/Object.cpp
        src/Loop.cpp
        src/Signal.cpp
        src/Thread.cpp
        src/Timer.cpp
        include/nx/core/detail/property_impl.hpp
)

find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS REQUIRED program_options system)

target_link_libraries(nx_core PUBLIC
        nx::macro
        spdlog
        Boost::program_options
        Boost::system
)

target_compile_options(nx_core PUBLIC "-Wno-pmf-conversions")

include(nxMakeModule)
nx_make_module(
        TARGET nx_core
        NAME core
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)


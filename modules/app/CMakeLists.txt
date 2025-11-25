add_library(nx_app
        src/App.cpp
        src/Object.cpp
        src/Thread.cpp
        src/Loop.cpp
        src/Signal.cpp
        src/Connection.cpp
        src/Dispatcher.cpp
        src/nxapp.cpp
        include/nx/app/Timer.hpp
        src/Timer.cpp
        include/nx/app/detail/property_defs.hpp
)

find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS REQUIRED program_options system)

target_link_libraries(nx_app PUBLIC
        nx::core
        spdlog
        Boost::program_options
        Boost::system
)

target_compile_options(nx_app PUBLIC "-Wno-pmf-conversions")

include(nxMakeModule)
nx_make_module(
        TARGET nx_app
        NAME app
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
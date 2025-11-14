add_library(nx_app
        src/App.cpp
        src/Object.cpp
        src/Thread.cpp
        src/Loop.cpp
        src/Signal.cpp
#        unused/Event.cpp
#        include/app/Thread.hpp
#        include/app/Loop.hpp
)

find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS REQUIRED program_options system)

target_link_libraries(nx_app PUBLIC
        nx::core
        spdlog
        Boost::program_options
        Boost::system
)

include(nxMakeModule)
nx_make_module(
        TARGET nx_app
        NAME app
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
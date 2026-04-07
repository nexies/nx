add_library(nx_core
        src/Connection.cpp
        src/object/object.cpp
        src/object/object_impl.p.cpp
        src/Loop.cpp
        src/Signal.cpp
        src/Thread.cpp
        src/Timer.cpp
        src/App.cpp

        include/nx/core/detail/property_impl.hpp
        include/nx/core/object/Property.hpp
        include/nx/core/detail/property_defs_cxx20.hpp
        include/nx/core/detail/property_defs_cxx20_parser.hpp
        include/nx/core/detail/object_defs.hpp

        include/nx/experimental/reflect
        include/nx/core/detail/enum_defs.hpp
        include/nx/core/detail/function_id.hpp
        include/nx/core/IODevice.hpp
        include/nx/core/detail/logger_defs.hpp
        include/nx/core/detail/units.hpp
        include/nx/core/app/Application.hpp
        include/nx/core/app/ProgramArguments.hpp
        src/app/Application.cpp
        src/app/AppFunctions.cpp
        include/nx/core/object/Object.hpp
        include/nx/core/object/ConnectionInfo.hpp
        include/nx/core/meta/MetaObject.hpp
        include/nx/core/meta/MetaProperty.hpp
        include/nx/core/meta/enum_flags.hpp
        include/nx/core/meta/enum.hpp

#        src/asio/context/context.cpp
#        src/asio/context/context_impl.hpp
#        src/asio/context/context_impl.cpp
#        src/asio/backend/backend.cpp
#        src/asio/backend/backend_epoll.inl
#        src/asio/backend/backend_iocp.inl
#        src/asio/backend/backend_kevent.inl
#        include/nx/core/detail/os_defs.hpp
#        include/nx/core/asio/steady_timer.hpp
#        src/asio/steady_timer.cpp
#        include/nx/core/asio/signal_set.hpp
#        include/nx/core/asio/backend/reactor_handle.hpp
#        src/asio/signal_set/signal_set.cpp
#        src/asio/signal_set/signal_set_impl.hpp
#        src/asio/signal_set/signal_set_impl.cpp
#        include/nx/core/asio/handle_notifier.hpp
#        src/asio/handle_notifier/handle_notifier.cpp
#        src/asio/handle_notifier/handle_notifier_impl.cpp
#        src/asio/handle_notifier/handle_notifier_impl.hpp
)

#if(WIN32)
#    set(Boost_DIR C:\\devel\\boost_1_75_0\\stage\\x86_64\\lib\\cmake)
#endif()

#find_package(Boost COMPONENTS REQUIRED program_options system HINTS C:\\devel\\boost_1_75_0\\stage\\x86_64\\lib\\cmake)

if (NX_ENABLE_CXX_20_FEATURES)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_features)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_reflect)
#    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_property)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_enum)
endif()

#find_package(spdlog)
find_package(spdlog REQUIRED)

target_link_libraries(nx_core PUBLIC
        nx::common
        nx::asio
        nx::macro
        spdlog::spdlog
)


target_compile_definitions(nx_core PUBLIC NX_DEVEL_LOGGING=1 NX_TRACE_SIGNALS=0)

if (MSVC)
    target_compile_options(nx_core PRIVATE /utf-8)
else()
    target_compile_options(nx_core PRIVATE -Wno-pmf-conversions)
endif()

#include(nxMakeModule)
#nx_make_module(
#        TARGET nx_core
#        NAME core
#        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

nx_configure_component(nx_core
        NAME core
)

add_subdirectory(tests)
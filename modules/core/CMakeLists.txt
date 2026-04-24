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
        include/nx/core2/object.hpp
        include/nx/core2/object/object.hpp
        include/nx/core2/object/meta_object.hpp
)

if (NX_ENABLE_CXX_20_FEATURES)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_features)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_reflect)
#    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_property)
    target_compile_definitions(nx_core PUBLIC -D__nx_cxx_20_enum)
endif()

target_link_libraries(nx_core PUBLIC
        nx::common
        nx::asio
        nx::macro
        nx::logging
)

target_compile_definitions(nx_core PUBLIC NX_DEVEL_LOGGING=1 NX_TRACE_SIGNALS=0)

if (MSVC)
    target_compile_options(nx_core PRIVATE /utf-8)
else()
    target_compile_options(nx_core PRIVATE -Wno-pmf-conversions)
endif()

nx_configure_component(nx_core
        NAME core
)

add_subdirectory(tests)
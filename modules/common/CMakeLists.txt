set(nx_common_includes
        include/nx/common/platform.hpp
        include/nx/common/platform/platform_defs.hpp
        include/nx/common/platform/standard_defs.hpp
        include/nx/common/platform/compiler_defs.hpp
        include/nx/common/platform/aarch_defs.hpp
        include/nx/common/platform/std_headers.hpp
        include/nx/common/platform/symbols_defs.hpp
        include/nx/common/platform/standard_defs.hpp

        include/nx/common/helpers.hpp
        include/nx/common/features.hpp
        include/nx/common/namespace.hpp

        include/nx/common/types.hpp
        include/nx/common/types/timing.hpp
        include/nx/common/types/source_location.hpp
        include/nx/common/types/result.hpp
        include/nx/common/types/singleton.hpp

        include/nx/common/types/errors.hpp
        include/nx/common/types/errors/error_descriptor.hpp
        include/nx/common/types/errors/error.hpp
        include/nx/common/types/errors/codes.hpp
        include/nx/common/types/errors/basic_error.hpp

        include/nx/common.hpp

        include/nx/common/format/fmt/format.h
        include/nx/common/format/format
#        include/nx/common/types/size.hpp
        include/nx/common/tweaks.hpp
        include/nx/common/types/size.hpp
)

set(nx_common_sources
        src/common.cpp
        src/types/errors/error_descriptor.cpp
        src/types/errors/error.cpp
        src/types/utf8.cpp
        include/nx/common/types/static_string.hpp
        include/nx/common/types/utf8.hpp
)


add_library(nx_common
        ${nx_common_includes}
        ${nx_common_sources}
)

target_include_directories(nx_common PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/nx/common/format>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/nx/common/format>
)

nx_configure_component(nx_common
        NAME common
        VERSION 0.0.1
)

add_subdirectory(tests)

# Bundled fmt (v12) uses two C++20 features broken in older MSVC:
#
#   FMT_USE_CONSTEVAL — safety net for VS2022+ where fmt would enable consteval.
#     VS2019 < 16.10 is already auto-disabled by fmt at base.h:129-130, but
#     VS2022 enables consteval and has its own constexpr downcast issues.
#
#   FMT_USE_NONTYPE_TEMPLATE_ARGS — MSVC defines __cpp_nontype_template_args in
#     C++17 mode but structural-type NTTPs are broken on older VS builds.
#
# The static_cast downcast bug in constexpr context (base.h:2468, 2477) is
# patched directly in the bundled fmt source with an FMT_MSC_VERSION guard.
if(MSVC)
    target_compile_definitions(nx_common PUBLIC
            FMT_USE_CONSTEVAL=0
            FMT_USE_CONSTEXPR=0
            FMT_USE_NONTYPE_TEMPLATE_ARGS=0
    )

    target_compile_options(nx_common PUBLIC
            /utf-8
    )
endif()


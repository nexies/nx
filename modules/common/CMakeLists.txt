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

        include/nx/common/types/errors.hpp
        include/nx/common/types/errors/error_descriptor.hpp
        include/nx/common/types/errors/error.hpp
        include/nx/common/types/errors/codes.hpp
        include/nx/common/types/errors/basic_error.hpp

        include/nx/common.hpp
)

set(nx_common_sources
        src/common.cpp
        src/types/errors/error_descriptor.cpp
        src/types/errors/error.cpp
)


add_library(nx_common
        ${nx_common_includes}
        ${nx_common_sources}
)

#nx_make_module(
#        TARGET nx_common
#        NAME common
#        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
#        VERSION 0.0.1
#)

nx_configure_component(nx_common
        NAME common
        VERSION 0.0.1
)

add_subdirectory(tests)
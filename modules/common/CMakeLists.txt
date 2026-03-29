add_library(nx_common
        include/nx/common/platform.hpp
        include/nx/common/platform/platform_defs.hpp
        include/nx/common/platform/standard_defs.hpp
        include/nx/common/platform/compiler_defs.hpp

        include/nx/common/features.hpp
        include/nx/common/types.hpp


        src/common.cpp
        include/nx/common/platform/aarch_defs.hpp
        include/nx/common/platform/std_headers.hpp
        include/nx/common/platform/symbols_defs.hpp
        include/nx/common/helpers.hpp
        include/nx/common/platform/standard_defs.hpp
        include/nx/common.hpp
        include/nx/common/types/timing.hpp
        include/nx/common/namespace.hpp
        include/nx/common/types/errors/basic_error.hpp
        include/nx/common/types/source_location.hpp
        tests/nx_common_test_source_location.cpp
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
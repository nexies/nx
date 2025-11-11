string(TIMESTAMP NX_BUILD_TIMESTAMP_UTC "%Y-%m-%d %H:%M:%S" UTC)
set(NX_GIT_SHA1 "unknown")
find_program(GIT_EXECUTABLE git)
if(GIT_EXECUTABLE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE NX_GIT_SHA1
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
endif()

set(NX_WITH_FMT 0)
set(NX_FMT_VERSION_STR "")
if(TARGET fmt::fmt)
    set(NX_WITH_FMT 1)
    if(DEFINED fmt_VERSION)
        set(NX_FMT_VERSION_STR "${fmt_VERSION}")
    else()
        get_target_property(_v fmt::fmt VERSION)
        if(_v)
            set(NX_FMT_VERSION_STR "${_v}")
        endif()
    endif()
endif()

set(NX_VERSION_STR  "${PROJECT_VERSION}")
set(NX_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(NX_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(NX_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(NX_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/nx/version.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/generated/nx/version.hpp"
        @ONLY
)

target_include_directories(nx
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
)
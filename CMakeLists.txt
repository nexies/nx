cmake_minimum_required (VERSION 3.20)
project (nx VERSION 0.3.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

set (NX_PROJECT_ROOT_DIR    ${CMAKE_CURRENT_SOURCE_DIR})
set (NX_CMAKE_MODULE_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (NX_NAMESPACE           nx)
set (NX_PROJECT_VERSION     ${PROJECT_VERSION})
set (NX_EXPORT_SET          NX_EXPORTED_TARGETS)
set (NX_GENERATED_INCLUDEDIR ${CMAKE_BINARY_DIR}/generated/${NX_NAMESPACE})
set (NX_CXX_STANDARD        17)
set (NX_PROJECT_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

option (NX_BUILD_DOCS           "Build docs"                                             OFF)
option (NX_BUILD_MODULES        "Build modules"                                          ON)
option (NX_BUILD_EXAMPLES       "Build examples"                                         OFF)
option (NX_BUILD_TESTS          "Enable building tests for the modules"                  OFF)
option (NX_BUILD_DEVEL_TESTS    "Enable building development tests"                      OFF)
option (NX_CLANG_TIDY           "Enable clang-tidy"                                      OFF)
option (NX_DEVEL_WARNING        "Enable devel warnings"                                  OFF)
option (NX_ENABLE_INSTALL       "Enable install targets"                                 ON)
option (NX_ENABLE_CXX_20_FEATURES "Enable std::c++20 features"                           OFF)

# ── Output directories ─────────────────────────────────────────────────────────
# Collect all executables and DLLs under bin/ so they can find each other at
# runtime without PATH/LD_LIBRARY_PATH tricks.  On multi-config generators
# (MSVC, Xcode) the per-config overrides prevent the generator from appending
# its own /Debug or /Release suffix.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

foreach (_cfg Debug Release RelWithDebInfo MinSizeRel)
    string (TOUPPER "${_cfg}" _CFG)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_CFG} "${CMAKE_BINARY_DIR}/bin")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_CFG} "${CMAKE_BINARY_DIR}/lib")
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_CFG} "${CMAKE_BINARY_DIR}/lib")
endforeach ()

# ── Modules ────────────────────────────────────────────────────────────────────
add_library (nx INTERFACE)
add_library (nx::nx ALIAS nx)

list (APPEND CMAKE_MODULE_PATH "${NX_CMAKE_MODULE_PATH}")
include (nxImportModules)

get_property (_components GLOBAL PROPERTY NX_ENABLED_COMPONENTS)
string (JOIN ", " _comp_str ${_components})
message ("[nx::nx] -- enabled components: ${_comp_str}")
foreach (comp IN LISTS _components)
    target_link_libraries (nx INTERFACE nx::${comp})
endforeach ()

# ── Install ────────────────────────────────────────────────────────────────────
if (NX_ENABLE_INSTALL)
    include (CMakePackageConfigHelpers)
    include (GNUInstallDirs)

    install (TARGETS nx EXPORT "${NX_EXPORT_SET}")

    # Build the component list for nxConfig.cmake at configure time.
    string (JOIN ";" NX_COMPONENTS_LIST ${_components})

    write_basic_package_version_file (
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file (
        cmake/nxConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nx"
    )

    install (FILES
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nx"
    )

    install (
        EXPORT      "${NX_EXPORT_SET}"
        NAMESPACE   nx::
        FILE        nxTargets.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/nx"
    )

    # Generated version headers (nx/<module>/version.hpp).
    install (DIRECTORY "${CMAKE_BINARY_DIR}/generated/nx"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()

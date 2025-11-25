cmake_minimum_required (VERSION 3.20)
project (nx VERSION 0.2.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (NX_PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (NX_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(NX_NAMESPACE nx)
set(NX_EXPORT_SET nxTargets)
set(NX_INSTALL_INCLUDEDIR include/nx)
set(NX_CXX_STANDARD 20)
set(NX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option (NX_BUILD_MODULE_CORE "Build nx::core module" ON)
option (NX_BUILD_MODULE_APP "Build nx::app module" ON)
option (NX_BUILD_TESTS "Build tests" OFF)

list (APPEND CMAKE_MODULE_PATH "${NX_MODULE_PATH}")


add_library (nx INTERFACE )
add_library (nx::nx ALIAS nx)

include(nxVersionInfo)

add_subdirectory(modules)

get_property(components GLOBAL PROPERTY NX_ENABLED_COMPONENTS)
foreach (comp IN LISTS components)
    target_link_libraries (nx INTERFACE nx::${comp})
    message (STATUS "Linking component nx::${comp} to nx::nx...")
endforeach ()


include (CMakePackageConfigHelpers)
install (
        TARGETS nx
        EXPORT nxTargets
)

write_basic_package_version_file (
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        cmake/nxConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
        INSTALL_DESTINATION lib/cmake/nx
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
        DESTINATION lib/cmake/nx
)

install(
        EXPORT nxTargets
        NAMESPACE nx::
        DESTINATION lib/cmake/nx
)

cmake_minimum_required (VERSION 3.20)
project (nx VERSION 0.3.1 LANGUAGES CXX)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (NX_PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (NX_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


set (NX_NAMESPACE nx)
set (NX_PROJECT_VERSION ${PROJECT_VERSION})
set (NX_EXPORT_SET NX_EXPORTED_TARGETS)
set (NX_INSTALL_INCLUDEDIR include/nx)
set (NX_GENERATED_INCLUDEDIR ${CMAKE_BINARY_DIR}/generated/${NX_NAMESPACE})
set (NX_CXX_STANDARD 17)
set (NX_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option (NX_MODULAR_NAMESPACE "Enable distinct namespace for each module" ON)
option (NX_BUILD_DOCS "Build docs" ON)
option (NX_BUILD_MODULES "Build modules" ON)
option (NX_BUILD_EXAMPLES "Build examples" ON)
option (NX_BUILD_TESTS "Enable building tests for the modules" ON)
option (NX_BUILD_DEVEL_TESTS "Enable building development tests for testing modules functionality" ON)
option (NX_CLANG_TIDY "Enable clang-tidy" ON)
option (NX_DEVEL_WARNING "Enable devel warnings" ON)
option (NX_ENABLE_INSTALL "Enable install" OFF)
option (NX_ENABLE_CXX_20_FEATURES "Enable std::c++20 features" OFF)

add_library (nx INTERFACE)
add_library (nx::nx ALIAS nx)

list (APPEND CMAKE_MODULE_PATH "${NX_CMAKE_MODULE_PATH}")
include(nxImportModules)

include(nxVersionInfo)

#add_subdirectory(third-party/spdlog)
#add_subdirectory(third-party/FTXUI)

#if(NX_BUILD_EXAMPLES)
#    add_subdirectory(examples)
#endif()

get_property(components GLOBAL PROPERTY NX_ENABLED_COMPONENTS)
string(JOIN ", " _comp_str ${components})
message("[nx::nx] -- enabled components: ${_comp_str}")
foreach (comp IN LISTS components)
    target_link_libraries (nx INTERFACE nx::${comp})
endforeach ()

include (CMakePackageConfigHelpers)
#install (
#        TARGETS nx
#        EXPORT NX_EXPORTED_TARGETS
#)

#write_basic_package_version_file (
#        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
#        VERSION ${PROJECT_VERSION}
#        COMPATIBILITY SameMajorVersion
#)

#configure_package_config_file(
#        cmake/nxConfig.cmake.in
#        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
#        INSTALL_DESTINATION lib/cmake/nx
#)

#install(FILES
#        "${CMAKE_CURRENT_BINARY_DIR}/nxConfig.cmake"
#        "${CMAKE_CURRENT_BINARY_DIR}/nxConfigVersion.cmake"
#        DESTINATION lib/cmake/nx
#)

#install(
#        EXPORT NX_EXPORTED_TARGETS
#        NAMESPACE nx::
#        DESTINATION lib/cmake/nx
#)

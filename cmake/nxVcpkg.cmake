# nxVcpkg.cmake
#
# Auto-configure the vcpkg toolchain on Windows when the user has not
# already provided CMAKE_TOOLCHAIN_FILE.
#
# Include this file BEFORE the first project() call:
#
#   cmake_minimum_required(VERSION 3.20)
#   include(cmake/nxVcpkg.cmake)
#   project(nx ...)
#
# vcpkg root is resolved in this order:
#   1. VCPKG_ROOT CMake variable (user-supplied)
#   2. VCPKG_ROOT environment variable
#   3. Common default install paths (C:/vcpkg, %USERPROFILE%/vcpkg)
#
# On non-Windows platforms this file is a no-op — OpenSSL and other
# dependencies are expected to come from the system package manager.

if(NOT WIN32)
    return()
endif()

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    return()
endif()

# ── Locate vcpkg root ──────────────────────────────────────────────────────────

if(NOT DEFINED VCPKG_ROOT)
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    elseif(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(VCPKG_ROOT "C:/vcpkg")
    elseif(EXISTS "$ENV{USERPROFILE}/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(VCPKG_ROOT "$ENV{USERPROFILE}/vcpkg")
    endif()
endif()

if(NOT DEFINED VCPKG_ROOT)
    message(WARNING
        "[nx] vcpkg not found on Windows.\n"
        "      Install vcpkg and either:\n"
        "        - set the VCPKG_ROOT environment variable, or\n"
        "        - pass -DVCPKG_ROOT=<path> to CMake.\n"
        "      Windows-only dependencies (e.g. OpenSSL) will not be available.")
    return()
endif()

set(_nx_vcpkg_toolchain "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

if(NOT EXISTS "${_nx_vcpkg_toolchain}")
    message(WARNING "[nx] vcpkg toolchain not found at: ${_nx_vcpkg_toolchain}")
    return()
endif()

set(CMAKE_TOOLCHAIN_FILE "${_nx_vcpkg_toolchain}"
    CACHE STRING "vcpkg toolchain (set by nxVcpkg.cmake)" FORCE)

message(STATUS "[nx] vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")

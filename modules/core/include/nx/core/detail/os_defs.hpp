//
// Created by nexie on 22.03.2026.
//

#ifndef NX_CORE_OS_DEFS_HPP
#define NX_CORE_OS_DEFS_HPP
// --- Определения операционной системы ---
#if defined(_WIN32) || defined(_WIN64)
    // Windows
    #define NX_OS_WINDOWS
    #define NX_OS_NAME "Windows"
#elif defined(__linux__)
    // Linux
    #define NX_OS_LINUX
    #define NX_OS_NAME "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
    // macOS
    #define NX_OS_MACOS
    #define NX_OS_NAME "macOS"
#elif defined(__FreeBSD__)
    // FreeBSD
    #define NX_OS_FREEBSD
    #define NX_OS_NAME "FreeBSD"
#elif defined(__unix__) || defined(__unix)
    // Unix-like OS
    #define NX_OS_UNIX
    #define NX_OS_NAME "Unix"
#else
    #error "Unknown operating system"
#endif

// --- Определения компилятора ---
#if defined(_MSC_VER)
    // Microsoft Visual C++
    #define NX_COMPILER_MSVC
    #define NX_COMPILER_NAME "MSVC"
#elif defined(__clang__)
    // Clang
    #define NX_COMPILER_CLANG
    #define NX_COMPILER_NAME "Clang"
#elif defined(__GNUC__) || defined(__GNUG__)
    // GCC (GNU Compiler Collection)
    #define NX_COMPILER_GCC
    #define NX_COMPILER_NAME "GCC"
#elif defined(__INTEL_COMPILER)
    // Intel Compiler
    #define NX_COMPILER_INTEL
    #define NX_COMPILER_NAME "Intel"
#else
    #error "Unknown compiler"
#endif

#endif //OS_DEFS_HPP

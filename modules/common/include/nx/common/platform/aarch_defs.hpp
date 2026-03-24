//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_AARCH_DEFS_HPP
#define NX_COMMON_AARCH_DEFS_HPP

#if defined(_M_X64) || defined(__x86_64__)
#   define NX_ARCH_X86_64 1
#   define NX_ARCH_NAME "x86_64"
#   define NX_ARCH_64BIT 1
#   define NX_ARCH_64 1
#elif defined(_M_IX86) || defined(__i386__)
#   define NX_ARCH_X86 1
#   define NX_ARCH_NAME "x86"
#elif defined(__aarch64__) || defined(_M_ARM64)
#   define NX_ARCH_64BIT 1
#   define NX_ARCH_64 1
#   define NX_ARCH_ARM64 1
#   define NX_ARCH_NAME "arm64"
#elif defined(__arm__) || defined(_M_ARM)
#   define NX_ARCH_ARM 1
#   define NX_ARCH_NAME "arm"
#elif defined(__riscv)
#   define NX_ARCH_RISCV 1
#   define NX_ARCH_NAME "riscv"
#endif

# ifndef NX_ARCH_NAME
#   define NX_ARCH_NAME "Unknown architecture"
# endif

#ifndef NX_ARCH_64
#   define NX_ARCH_64 1
#   define NX_ARCH_32 1
#endif
#endif //NX_COMMON_AARCH_DEFS_HPP
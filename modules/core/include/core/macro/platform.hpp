//
// Created by nexie on 22.11.2025.
//

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#define NX_EXPAND(...) __VA_ARGS__

#define __NX_CONCAT_(x, y) x##y
#define __NX_CONCAT_3(a, b, c) a##b##c
#define __NX_CONCAT_4(a, b, c, d) a##b##c##d
#define __NX_CONCAT_5(a, b, c, d, e) a##b##c##d##e

#define NX_CONCAT(...) __NX_CONCAT_(__VA_ARGS__)
#define NX_CONCAT_3(...) __NX_CONCAT_3(__VA_ARGS__)
#define NX_CONCAT_4(...) __NX_CONCAT_4(__VA_ARGS__)
#define NX_CONCAT_5(...) __NX_CONCAT_5(__VA_ARGS__)

#define __NX_RCONCAT_(x, y) y##x
#define NX_RCONCAT(...) __NX_RCONCAT_(__VA_ARGS__)

#define NX_CONSUME(...)

#define __NX_TO_STRING_(v) #v
#define NX_TO_STRING(...) __NX_TO_STRING_(__VA_ARGS__)
#define NX_TO_STRING_FLAT(v) #v

#endif //PLATFORM_HPP

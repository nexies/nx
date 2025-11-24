//
// Created by nexie on 22.11.2025.
//

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#define NX_EXPAND(...) __VA_ARGS__

#define __NX_CONCAT_(x, y) x##y
#define NX_CONCAT(...) __NX_CONCAT_(__VA_ARGS__)

#define __NX_RCONCAT_(x, y) y##x
#define NX_RCONCAT(...) __NX_RCONCAT_(__VA_ARGS__)

#define NX_CONSUME(...)

#endif //PLATFORM_HPP

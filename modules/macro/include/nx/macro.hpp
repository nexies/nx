//
// Created by nexie on 11.12.2025.
//

#ifndef NXTBOT_MACRO_HPP
#define NXTBOT_MACRO_HPP

// #define NX_MAX_NUMBER

#ifndef NX_MAX_NUMBER
#define NX_MAX_NUMBER 64
#endif

#include <string>
#include "macro/util/platform.hpp"

namespace nx::macro
{
    std::string version ();
}

#endif //NXTBOT_MACRO_HPP
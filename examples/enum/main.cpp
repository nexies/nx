//
// Created by nexie on 10.02.2026.
//

#include <nx/core/detail/enum_defs.hpp>
#include <iostream>

#include "nx/core/detail/logger_defs.hpp"


enum MyFlag
{
    E1 = 1,
    E2 = 2,
    E3 = 4,
    E4 = 8
};

// NX_ENUM(MyFlag)

// using MyFlags = make_flags_from_enum<MyFlag>;



int main (int argc, char * argv[])
{
    // MyFlags f = MyFlag::E1 | MyFlag::E4;
    // MyFlags f2 = MyFlag::E2 | MyFlag::E1;
    // MyFlags f3 = MyFlag::E3 | MyFlag::E2;

    // nxInfo("{:q}", MyFlag::E1);

    // std::cerr << fmt::format("{}", MyFlag::E1) << std::endl;
    return 0;
}
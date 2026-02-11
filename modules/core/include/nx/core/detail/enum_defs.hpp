//
// Created by nexie on 10.02.2026.
//

#ifndef NXTBOT_ENUM_DEFS_HPP
#define NXTBOT_ENUM_DEFS_HPP

#ifndef __nx_cxx_20_enum
#error ""
#endif

#include <nx/experimental/reflect>

namespace test
{
    enum MyEnum
    {
        MyValue1,
        MyValue2,
        MyValue3
    };

    std::string_view to_string(MyEnum e)
    {
        return reflect::enum_name(e);
    }
};

#endif //NXTBOT_ENUM_DEFS_HPP
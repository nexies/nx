//
// Created by nexie on 10.02.2026.
//

#include <nx/core/detail/enum_defs.hpp>
#include <iostream>


enum MyFlag
{
    E1 = 1,
    E2 = 2,
    E3 = 4,
    E4 = 8
};

using MyFlags = make_flags_from_enum<MyFlag>;

constexpr MyFlags operator|(const MyFlag a, const MyFlag b) noexcept {
    return MyFlags {static_cast<MyFlags::underlying_type>(a) | static_cast<MyFlags::underlying_type>(b) };
}
constexpr MyFlags operator&(const MyFlag a, const MyFlag b) noexcept {
    return MyFlags {static_cast<MyFlags::underlying_type>(a) & static_cast<MyFlags::underlying_type>(b) };
}
constexpr MyFlags operator^(const MyFlag a, const MyFlag b) noexcept {
    return MyFlags {static_cast<MyFlags::underlying_type>(a) ^ static_cast<MyFlags::underlying_type>(b) };
}

int main (int argc, char * argv[])
{
    MyFlags f = MyFlag::E1 | MyFlag::E4;
    MyFlags f2 = MyFlag::E2 | MyFlag::E1;
    MyFlags f3 = MyFlag::E3 | MyFlag::E2;
    return 0;
}
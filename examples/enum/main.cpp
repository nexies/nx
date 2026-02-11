//
// Created by nexie on 10.02.2026.
//

#include <nx/core/detail/enum_defs.hpp>
#include <iostream>

enum MyEnum
{
    Value1,
    Value2,
    Value3,
};

int main (int argc, char * argv[])
{
    std::cout << reflect::enum_name(MyEnum::Value1) << std::endl;

    return 0;
}
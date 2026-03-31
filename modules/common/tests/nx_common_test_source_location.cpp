//
// Created by nexie on 30.03.2026.
//

#include <nx/common/types/source_location.hpp>
#include <iostream>

template<typename Type>
void foo () {
    std::cerr << nx::source_location().function() << std::endl;
    std::cerr << NX_FUNCTION_SIGNATURE << std::endl;
}

struct src_wrap {
    nx::source_location loc;

    explicit
    src_wrap(nx::source_location loc = nx::source_location())
        : loc(loc)
    {}
};

struct cls {
    static void foo () {
        std::cerr << nx::source_location().function() << std::endl;
    }
};

void throw_source_location ()
{
    auto loc = new nx::source_location;

    throw loc;
}

void throw_source_location1 ()
{
    throw_source_location ();
}

void throw_source_location2 ()
{
    throw_source_location1 ();
}

void throw_source_location3 ()
{
    throw_source_location2 ();
}

const auto g_log = nx::source_location();

int main (int argc, char * argv []) {
    nx::source_location src;
    std::cerr << src << std::endl;

    std::cerr << nx::source_location::current().column() << std::endl;

    foo<int>();

    try
    {
        throw_source_location3();
    } catch ( nx::source_location * loc )
    {
        std::cerr << "Throw At " << *loc << std::endl;
    }

    return 0;
}
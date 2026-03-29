//
// Created by nexie on 30.03.2026.
//

#include <nx/common/types/source_location.hpp>
#include <iostream>

template<typename Type>
void foo () {
    std::cerr << nx::source_location().function() << std::endl;
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
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

const auto g_log = nx::source_location();

int main (int argc, char * argv []) {
    nx::source_location src;
    std::cerr << src.function() << std::endl;

    src_wrap w;
    std::cerr << w.loc.function() << std::endl;

    cls::foo();


    std::cerr << g_log.file() << std::endl;

    foo<int>();
}
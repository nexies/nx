//
// Created by nexie on 06.04.2026.
//

#include <iostream>
#include <nx/common/types/result.hpp>

nx::result<const char *>
foo (int input) {
    if (input <= 10)
        return nx::err::invalid_argument("Input should be more than 10");
    return "HAHA HAHA";
}

int main() {
    auto res = foo(9);
    if (!res)
        nx::explain(res.error());

    res = foo (15);
    std::cerr << "Result: " << res.value() << std::endl;
    return 0;
}
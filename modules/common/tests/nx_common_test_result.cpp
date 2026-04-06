//
// Created by nexie on 06.04.2026.
//

#include <iostream>
#include <nx/common/types/result.hpp>


nx::result<int>
foo (int input) {
    if (input <= 10)
        return nx::err::invalid_argument("Input should be more than 10");
    return input - 10;
}

int main() {
    auto res = foo(5)
        .expect([](auto er) {
            nx::explain(er); return -1;
        });

    std::cerr << res << std::endl;
    auto res2 = foo (15);
    std::cerr << "Result: " << res2 << std::endl;
    return 0;
}
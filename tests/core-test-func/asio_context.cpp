//
// Created by nexie on 22.03.2026.
//

#include <nx/core/asio/context/context.hpp>
#include <iostream>

using namespace nx;
asio::Context ctx;

void task2();

void task1() {
    static int counter = 0;

    std::cerr << "Task 1; counter = " << counter << std::endl;
    counter += 1;
    if (counter < 10)
        ctx.post(task2);
    else
        ctx.stop();

    return;
}

void task2() {
    std::cerr << "Task 2" << std::endl;
    ctx.post(task1);
}


int main (int argc, char * argv[]) {

    ctx.post(task1);
    auto n = ctx.run();

    std::cerr << "Executed " << n << " tasks" << std::endl;
    return 0;
}
//
// Created by nexie on 28.03.2026.
//

#include <nx/asio/context/io_context.hpp>
#include <nx/asio/signal_set.hpp>
#include <iostream>

int main (int argc, char * argv[]) {
    using namespace nx::asio;

    io_context ctx;

    signal_set signals (ctx);
    signals.add(SIGINT);
    signals.add(SIGTERM);
    // signals.add(SIGPIPE);
    signals.async_wait([&] (nx::result<int> res) {
        if (res)
        {
            std::cerr << "Received signal " << res.value() << std::endl;
            ctx.stop();
            return;
        }
        std::cerr << "SIGNAL ERROR" << std::endl;
        nx::explain(res.error());
    });

    ctx.post([]{std::cerr << "task 1" << std::endl; });
    ctx.post([]{std::cerr << "task 2" << std::endl; });
    ctx.post([]{std::cerr << "task 3" << std::endl; });
    ctx.post([]{std::cerr << "task 4" << std::endl; });

    int n {};
    n += ctx.poll();
    std::cerr << "executed " << n << " task" << std::endl;
    n += ctx.run();

    std::cerr << "run " << n << " tasks" << std::endl;

    return 0;
}

//
// Created by nexie on 19.02.2026.
//

#include <../../modules/core/include/nx/core/App.hpp>

#include "nx/core/Timer.hpp"

struct TimeoutReceiver : public nx::Object
{
    void onTimeout()
    {
        nxInfo("Timeout!");
    }

    NX_SIGNAL(receivedTimeout);
};


int main(int argc, char * argv[]) {
    using namespace nx;

    App::Init(argc, argv);

    Timer timer;
    TimeoutReceiver receiver;

    nx::connect(&timer, &Timer::timeout,
        &receiver, &TimeoutReceiver::receivedTimeout, Connection::Queued);

    nx::connect(&receiver, &TimeoutReceiver::receivedTimeout,
                &receiver, &TimeoutReceiver::onTimeout);

    timer.setDuration(Milliseconds(500));
    timer.setType(Timer::Type::Periodic);
    timer.startNow();

    return App::Exec();
}
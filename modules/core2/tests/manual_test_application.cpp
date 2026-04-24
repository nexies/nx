//
// Created by nexie on 4/24/2026.
//

#include <nx/core2/app/application.hpp>

#include "nx/core2/object/object.hpp"
#include "nx/core2/detail/logger_defs.hpp"
#include "nx/core2/timer/timer.hpp"

struct cls : public nx::core::object
{
    NX_OBJECT(cls)

    NX_PROPERTY(ticks, TYPE int, READ, WRITE, NOTIFY, DEFAULT 0)

    void on_timeout ()
    {
        set_ticks(get_ticks() + 1);
    }
};

struct sink : public nx::core::object
{
    NX_OBJECT(sink)

    cls & c;

    sink (cls & c)
        : c(c)
    {
        nx::core::connect(
            &c, &cls::ticks_changed,
            this, &sink::on_ticks_changed
            );
    }

    void on_ticks_changed()
    {
        nxDebug("Ticks changed: {}", c.get_ticks());
    }
};

int main (int argc, char * argv[])
{
    nx::core::application a (argc, argv);

    nx::core::timer tim;
    cls c;

    nx::core::connect(&tim, &nx::core::timer::timeout,
        &c, &cls::on_timeout);

    sink s(c);

    tim.set_interval(std::chrono::milliseconds(1000));
    tim.set_type(nx::core::timer::timer_type::periodic);
    tim.start();

    a.exec();
}
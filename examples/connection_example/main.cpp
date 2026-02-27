//
// Created by nexie on 27.02.2026.
//

#include <nx/core.hpp>
#include <nx/core/Functor.hpp>

using namespace nx;

class BaseObject : public Object
{
public:

    void slot ()
    {
        nxDebug("Invoked slot on BaseObject");
    }
};


class DerivedObject : public BaseObject
{
public:
    void anotherSlot ()
    {
        nxDebug("Invoked anotherSlot on DerivedObject");
    }
};

int main (int argc, char * argv [])
{
    App::Init(argc, argv);

    Timer timer;
    BaseObject base;
    DerivedObject derived;

    connect(&timer, &Timer::timeout, &base, &BaseObject::slot );
    connect(&timer, &Timer::timeout, &derived, &DerivedObject::slot );
    connect(&timer, &Timer::timeout, &derived, &BaseObject::slot );
    connect(&timer, &Timer::timeout, &derived, &DerivedObject::anotherSlot );

    // connect(&timer, &Timer::timeout, &derived, &BaseObject::slot );

    // auto & tf = &Timer::timeout;

    // using a = nx::detail::FunctionDescriptor<void (DerivedObject::*&)()>::MemberOf;

    // auto df = &DerivedObject::slot;



    timer.setType(Timer::Type::Periodic);
    timer.setDuration(Seconds(1));

    timer.startNow();

    return App::Exec();
}
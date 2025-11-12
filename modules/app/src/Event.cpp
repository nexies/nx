//
// Created by nexie on 12.11.2025.
//

#include "app/Event.hpp"
#include "app/nxapp.hpp"


nx::Event::Event(Type type) :
    _type (type)
{

}

void nx::Event::accept()
{
    _accepted = true;
}


nx::SignalEvent::SignalEvent(std::thread::id target, InvokerPtr invoker) :
    Event(Event::Signal),
    _thread_id(target),
    _invoker(invoker)
{

}

void nx::SignalEvent::accept()
{
    if (std::this_thread::get_id() == _thread_id)
    {
        _invoker->invoke();
        Event::accept();
    }

    nxError("SignalEvent::accept() - not the target thread!");
}

std::thread::id nx::SignalEvent::targetThread() const
{
    return _thread_id;
}

//
// Created by nexie on 09.11.2025.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "nxapp.hpp"
#include "Event.hpp"

namespace nx {

    class Thread;

    class Object {
    public:
        Object ();
        virtual ~Object ();

        ThreadId attachedThreadId () const;
        Result attachToThread (Thread *);

        virtual Result onEvent (Event *);
        Result notify (Object *, Event *) const;

        virtual Result onUpdate (Event *);
        virtual Result onTimer (TimerEvent *);

    protected:
        void     _generateEvent (Object *, Event *, int priority = 0) const;
        Thread * _getLocalThread () const;
        void _reattachToLocalThread ();
        void _reattachToThread (Thread *);
    private:
        Thread * local_thread;
    };
}

#endif //OBJECT_HPP

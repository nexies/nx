//
// Created by nexie on 09.11.2025.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "nxapp.hpp"

namespace nx {

    class Loop;
    class Event;

    class Object {
    public:
        Object ();
        virtual ~Object ();

    protected:
        virtual Result _onEvent (Event *);
        void   _generateEvent (Event *) const;
        Loop * _getLocalLoop () const;
    private:

    };
}

#endif //OBJECT_HPP

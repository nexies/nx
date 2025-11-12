//
// Created by nexie on 09.11.2025.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Signal.hpp"
#include "Connection.hpp"

namespace nx {

    class EventLoop;

    class Object {
    public:
        Object ();
        Object (const std::string & name);

        std::string objectName() const;
        void setObjectName(const std::string & name);

    public: //TODO: signals
        NX_SIGNAL(objectNameChanged, const char *)
        void onObjectNameChanged(const char * name) {

        }
    private:
        class Impl;
        Impl * _impl;
        EventLoop * _getThisEventLoop();
    };
}

#endif //OBJECT_HPP

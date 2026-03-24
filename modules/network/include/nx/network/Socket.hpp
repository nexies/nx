//
// Created by nexie on 21.02.2026.
//

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <nx/core/IODevice.hpp>
#include <sys/socket.h>

namespace nx {
    class Socket : public IODevice {
        public:
        // virtual bool bind (std::string_view addr, uint16_t port) = 0;

        NX_SIGNAL(socketError, Result)

        virtual Result open () = 0;
        virtual void close () = 0;
    };
}

#endif //SOCKET_HPP

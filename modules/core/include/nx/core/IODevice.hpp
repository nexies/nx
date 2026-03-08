//
// Created by nexie on 21.02.2026.
//

#ifndef IODEVICE_HPP
#define IODEVICE_HPP

#include <nx/core/Object.hpp>

namespace nx {

    class IODevice : public Object {

    public:

        NX_SIGNAL(bytesWritten, uint64_t)
        NX_SIGNAL(readyRead)
        NX_SIGNAL(readyReadBytes, uint64_t)

        // int read (char * buf, int len = -1);
        // int write (char * data, int len = -1);

    protected:
        virtual int _readData (char * buf, int len) = 0;
        virtual int _writeData (char * buf, int len) = 0;
    };

}

#endif //IODEVICE_HPP

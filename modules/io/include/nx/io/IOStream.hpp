//
// Created by nexie on 10.02.2026.
//

#ifndef NXTBOT_IOSTREAM_HPP
#define NXTBOT_IOSTREAM_HPP

#include <nx/core/Object.hpp>

namespace nx
{
    class IOStream : public Object
    {
    public:

        /// readyRead SIGNAL
        NX_SIGNAL(readyRead)

        ///
        /// @param _arg1
        NX_SIGNAL(bytesWritten, size_t)

    protected:
        /// Stream read implementation
        /// @param buf - buffer to read to
        /// @param len - amount of bytes to read
        /// @return amount of bytes that were really read
        virtual int _read (char * buf, int len) = 0;

        /// Stream write implementation
        /// @param buf - buffer to write from
        /// @param len - amount of bytes to write
        /// @return amount of bytes that were really written
        virtual int _write (char * buf, int len) = 0;
    };
}

#endif //NXTBOT_IOSTREAM_HPP
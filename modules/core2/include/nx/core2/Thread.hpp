//
// Created by nexie on 01.12.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include <boost/asio/io_context.hpp>

namespace nx
{
    class Thread;

    namespace detail
    {
        class ThreadContext
        {
            friend class Thread;
            using Context = boost::asio::io_context;
            Context context;

        };
    }

    class Thread
    {

    };
}

#endif //THREAD_HPP

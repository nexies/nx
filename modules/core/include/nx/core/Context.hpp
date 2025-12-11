//
// Created by nexie on 05.12.2025.
//

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <boost/asio/io_context.hpp>

namespace nx
{
    namespace detail
    {
        class Context
        {
            boost::asio::io_context io;
        private:

        };
    }
}

#endif //CONTEXT_HPP

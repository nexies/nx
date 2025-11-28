//
// Created by nexie on 28.11.2025.
//

#include "nx/app/ThreadContext.hpp"
#include <boost/asio/post.hpp>
#include <boost/bind/bind.hpp>

using namespace nx;

void ThreadContext::schedule(std::function<void()>&& work)
{

}

void ThreadContext::schedule(Signal&& signal)
{
    boost::asio::post(io_context, std::move(signal));
}

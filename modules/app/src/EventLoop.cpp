//
// Created by nexie on 09.11.2025.
//

#include "app/EventLoop.hpp"

#include "tgbot/net/BoostHttpOnlySslClient.h"


void nx::EventLoopDispatcherInstance::addEventLoop(id p_id, EventLoop* p_loop)
{
    std::lock_guard<std::mutex> lock(_m);
    _d[p_id] = p_loop;
}

void nx::EventLoopDispatcherInstance::removeEventLoop(id p_id)
{
    std::lock_guard<std::mutex> lock(_m);
    _d.erase(p_id);
}

nx::EventLoop* nx::EventLoopDispatcherInstance::getEventLoop(id p_id)
{
    if (auto const it = _d.find(p_id); it != _d.end())
        return it->second;
    return nullptr;
}

//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_SIGNAL_SET_HPP
#define NX_CORE_ASIO_SIGNAL_SET_HPP
#include <functional>
#include <memory>

namespace nx::asio
{
    class Context;

    class SignalSet
    {
    public:
        using HandlerType = std::function<void(int)>;

        explicit
        SignalSet(Context & ctx);

        ~SignalSet();

        SignalSet(const SignalSet &) = delete;
        SignalSet & operator=(const SignalSet &) = delete;

        void add(int signal);
        void remove(int signal);
        void clear();

        template <typename Handler>
        void asyncWait(Handler && handler)
        {
            _asyncWaitImpl(HandlerType(std::forward<Handler>(handler)));
        }

        std::size_t cancel();

    private:
        void _asyncWaitImpl(HandlerType handler);

        class Impl;
        std::shared_ptr<Impl> impl_;
    };
}

#endif //NX_SIGNAL_SET_HPP
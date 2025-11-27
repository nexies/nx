//
// Created by nexie on 19.11.2025.
//

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <boost/asio/io_service.hpp>

#include "nx/app/Loop.hpp"
#include "nx/app/Thread.hpp"
#include "nx/core/TimerWheel.h"

namespace nx {

    class PollService {
    public:
        virtual ~PollService() = default;

        virtual Result init ();
        virtual Result cleanup ();

        virtual bool isInit() const;
        virtual bool initFailed () const;

        virtual size_t poll ();
        virtual size_t pollFor (Duration dur);
        virtual bool pollOne() = 0;
    };

    class BoostPollService : public PollService {
    public:
        explicit BoostPollService (boost::asio::io_service& io_service);
        bool pollOne () override;
    private:
        boost::asio::io_service& io_service;
    };

    class SystemSignalPollService final : public  PollService {
        static std::atomic_int s_exit_signals;
        static std::atomic_int s_abort_signals;
        static void system_signal_handler (int signal);
        Result _installSignalHandler ();
    public:
        Result init () override;
        Result cleanup () override;
        bool pollOne () override;
    private:
        bool is_init { false };
    };

    // TODO: class TimersPollService final : public  PollService {};

    class PollLoop final : public Loop {
        std::set<std::shared_ptr<PollService>> & services;
        // // Duration single_poll_duration { Milliseconds(16) }; // ~ 60 times per second
        // Duration single_poll_duration { Milliseconds(1000) }; // ~ 60 times per second
    public:
        PollLoop (std::set<std::shared_ptr<PollService>> & services);
        Result exec () override;
        Result processEvents() override;

        // bool addService (std::shared_ptr<PollService> service);
        // bool removeService (std::shared_ptr<PollService> service);

    private:
        bool _doServicePoll (std::shared_ptr<PollService> & service, Duration timeout);
    };

    class PollThread final : public Thread {
        std::set<std::shared_ptr<PollService>> services;
    public:
        PollThread ();
        Result execute () override;

        bool addService (std::shared_ptr<PollService> service);
        bool removeService (std::shared_ptr<PollService> service);
    };

    class MainDispatcher final : public Thread {

    public:
        ~MainDispatcher() override = default;
        Result execute () override;

        TimerId addTimer(TimerType, Duration, detail::timer_callback_t);
        Result cancelTimer(TimerId);




    protected:
        void _installSignalHandlers ();

        void _scanExitSignals ();
        void _rotateTimers ();
        void _scanInputChars ();

    private:
        TimerWheel<1024> timerWheel;
    };

}

#endif //DISPATCHER_HPP

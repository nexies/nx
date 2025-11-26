// //
// // Created by nexie on 25.11.2025.
// //
//
// #ifndef TIMER_HPP
// #define TIMER_HPP
//
// #include "nx/app/Object.hpp"
// #include "nx/core/Singleton.hpp"
//
// #include "boost/asio.hpp"
// #include "boost/bind.hpp"
//
// namespace nx
// {
//     namespace detail
//     {
//         static constexpr TimerId g_invalidTimerId = std::numeric_limits<TimerId>::min();
//         class TimerManagerInstance : public Object
//         {
//             TimerManagerInstance(boost::asio::io_service& io_service);
//
//             static size_t Poll();
//         private:
//
//             void add_timer(int interval_seconds) {
//                 boost::shared_ptr<boost::asio::deadline_timer> timer(
//                     new boost::asio::deadline_timer(io_service, boost::posix_time::seconds(interval_seconds)));
//
//
//                 // timer->async_wait(boost::bind(&TimerManagerInstance::on_timer_elapsed, this, timer));
//                 // timers.push_back(timer);
//             }
//
//             static TimerId _NextTimerId ();
//             static TimerManagerInstance & _Self();
//
//             boost::asio::io_service& io_service;
//             std::vector<boost::shared_ptr<boost::asio::deadline_timer>> timers;
//         };
//         using TimerManager = Singleton<TimerManagerInstance> ;
//
//     }
//
//     class Timer : public Object
//     {
//     public:
//         Timer ();
//         ~Timer () override;
//
//         bool isRunning () const;
//         bool isPaused () const;
//
//         bool isSingleShot () const;
//         bool isPeriodic () const;
//
//         Duration timeLeft () const;
//         TimerId getId() const;
//
//         void pause () const;
//         void unpause () const;
//
//         TimerId start ();
//         bool stop ();
//
//         void duration () const;
//         void setDuration (Duration duration);
//
//         void setSingleShot (bool singleshot);
//
//     public: // signals
//         NX_SIGNAL(expired)
//         NX_SIGNAL(started)
//         NX_SIGNAL(stopped)
//         NX_SIGNAL(durationChanged, Duration)
//
//     private:
//         TimerId _start();
//         bool _stop(TimerId);
//
//         TimerId id;
//         bool running;
//     };
// }
//
// #endif //TIMER_HPP

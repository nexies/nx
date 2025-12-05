//
// Created by nexie on 12.11.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include <stack>

#include "nx/app/Object.hpp"
#include "nx/app/Signal.hpp"
#include "nx/core/Singleton.hpp"

#include <thread>
#include <unordered_map>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include "tgbot/Api.h"

namespace nx
{
    namespace detail
    {
        class ThreadInfoInstance;
        constexpr ThreadId g_invalidThreadId = std::numeric_limits<ThreadId>::max();
    }

    class Signal;
    class Loop;

    class Thread : public Object
    {
        friend class ::nx::detail::ThreadInfoInstance;
        friend class ::nx::Signal;
        friend class ::nx::Loop;

        using Context = boost::asio::io_context;
        struct ContextLocker
        {
            using Guard = boost::asio::executor_work_guard<Context::executor_type>;
            std::unique_ptr<Guard> guard { nullptr };
            Context & context;

            explicit ContextLocker(Context & context) : context(context) {};
            void lock ()
            {
                if (!guard)
                    guard = std::make_unique<Guard>(boost::asio::make_work_guard(context));
            }
            void unlock ()
            {
                if (guard)
                {
                    guard->reset();
                    guard.reset(nullptr);
                }
            }
        };

    public:
        Thread();
        ~Thread() override;

        virtual Result start();
        virtual Result execute();

        [[nodiscard]]
        ThreadId getId() const;
        [[nodiscard]]
        NativeThreadId getNativeId() const;

        // bool pushSignal (Signal && signal, int priority);

        bool schedule(Signal&& signal);
        bool schedule(Signal&& signal, Duration delay);

        bool isRunning() const;
        bool isSleeping() const;

        virtual void sleep(Duration);
        void sleepUntil(TimePoint);

        void quit();
        void exit(int);
        void exitAndWait(int);
        void terminate(); //TODO: Terminate the working thread

        NX_SIGNAL(aboutToQuit)

        bool waitForExit(); //TODO: Wait until thread exists

        static Thread* Current ();
        static Thread* FromCurrentThread ();
        static ThreadId CurrentId ();
        static Loop* CurrentLoop ();
        static Context & CurrentContext ();

        Loop* loop() const;
        Context & context();

        void setSigmaskBlock(sigset_t& set);
        void setSigmaskAllow(sigset_t& set);

    protected:
        void _sleepImpl(Duration);
        virtual Result _startExecute();
        Loop * _topLoop () const;

        NativeThreadId native_id;
        ThreadId id{detail::g_invalidThreadId};
        std::unique_ptr<std::thread> thread;
        std::atomic_bool running{false};
        std::atomic_bool sleeping{false};

        // SignalQueue signal_queue;
        // Loop* current_loop{nullptr};
        Context io_context;
        ContextLocker context_locker { io_context };
        std::stack<Loop *> loops;

    private:
    };

    class LocalThread : public Thread
    {
    public:
        LocalThread();
        ~LocalThread() override;
        Result start() override final;

    protected:
        Result _startExecute() override final;
    };

    namespace detail
    {
        class ThreadInfoInstance
        {
            friend class Thread;

            std::unordered_map<ThreadId, Thread*> threads_by_id;
            std::unordered_map<NativeThreadId, Thread*> threads_by_native_id;

            ThreadId _getNextThreadId() const;

        public:
            ~ThreadInfoInstance();

            ThreadId registerThread(Thread* thread);
            bool registerThreadNativeId(NativeThreadId, Thread*);
            bool unregisterThreadNativeId(NativeThreadId);
            bool unregisterThread(ThreadId threadId);


            Thread* threadForId(ThreadId);
            Thread* threadForNativeId(NativeThreadId);

            size_t threadCount() const;

            void exitAllThreads();
            void waitForAllThreadsExit();
        };

        typedef ::nx::Singleton<ThreadInfoInstance> ThreadInfo;
    }
}

#endif //THREAD_HPP

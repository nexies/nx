//
// Created by nexie on 12.11.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include "Object.hpp"
#include "Signal.hpp"
#include "core/Singleton.hpp"

#include <thread>


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

    public:
        Thread();
        ~Thread () override;

        virtual Result start ();
        virtual Result execute ();

        [[nodiscard]]
        ThreadId getId () const;
        [[nodiscard]]
        NativeThreadId getNativeId () const;

        bool pushSignal (Signal && signal, int priority);

        bool isRunning () const;
        bool isSleeping () const;

        virtual void sleep (Duration);
        void sleepUntil (TimePoint);

        void quit ();
        void exit (int);
        void terminate ();                  //TODO: Terminate the working thread

        NX_SIGNAL(aboutToQuit)

        bool waitForExit ();                //TODO: Wait until thread exists

        static Thread * current ();
        static Thread * fromCurrentThread ();
        static ThreadId currentId ();
        static Loop * currentLoop ();
        static SignalQueue * currentQueue ();

        Loop * loop () const;
        SignalQueue * queue();

        void setSigmaskBlock (sigset_t & set);
        void setSigmaskAllow (sigset_t & set);
    protected:

        void _sleepImpl (Duration);
        virtual Result _startExecute ();

        NativeThreadId native_id;
        ThreadId id { detail::g_invalidThreadId };
        std::atomic_bool running { false };
        std::atomic_bool sleeping { false };
        SignalQueue signal_queue;
        Loop * current_loop { nullptr };
        std::unique_ptr<std::thread> thread;

    private:
    };

    class LocalThread : public Thread
    {
    public:
        LocalThread();
        ~LocalThread() override;
        Result start () override final;
    protected:
        Result _startExecute () override final;
    };

    namespace detail
    {

        class ThreadInfoInstance
        {
            friend class Thread;

            std::unordered_map<ThreadId, Thread*> threads_by_id;
            std::unordered_map<NativeThreadId, Thread*> threads_by_native_id;

            ThreadId _getNextThreadId () const;
        public:
            ~ThreadInfoInstance ();

            ThreadId registerThread (Thread * thread);
            bool registerThreadNativeId (NativeThreadId, Thread*);
            bool unregisterThreadNativeId (NativeThreadId);
            bool unregisterThread (ThreadId threadId);


            Thread * threadForId(ThreadId);
            Thread * threadForNativeId (NativeThreadId);

            void exitAllThreads ();
            void waitForAllThreadsExit ();
        };

        typedef ::nx::Singleton<ThreadInfoInstance> ThreadInfo;
    }
}

#endif //THREAD_HPP

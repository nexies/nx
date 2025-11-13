//
// Created by nexie on 12.11.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include "Object.hpp"
#include "Event.hpp"
#include <thread>

#include "core/Singleton.hpp"

namespace nx
{
    namespace detail
    {
        class ThreadInfoInstance;
        constexpr ThreadId g_invalidThreadId = std::numeric_limits<ThreadId>::max();
    }

    class Thread : public Object
    {
        friend class ::nx::detail::ThreadInfoInstance;

    public:
        Thread();
        ~Thread () override;

        virtual Result start ();
        virtual Result execute ();

        [[nodiscard]]
        ThreadId getId () const;
        [[nodiscard]]
        NativeThreadId getNativeId () const;

        bool pushEvent (Object * obj, Event * event, int priority);

        bool isRunning () const;
        bool isSleeping () const;

        virtual void sleep (Duration);      //TODO: Via event loop
        void sleepUntil (TimePoint);        //TODO: Via event loop

        void quit ();                       //TODO: Send event to event loop
        void exit ();                       //TODO: Stop the loop execution (gracefully)
        void terminate ();                  //TODO: Terminate the working thread

        bool waitForExit ();                //TODO: Wait until thread exists

        static Thread * current ();
        static Thread * fromCurrentThread ();

        EventQueue * getQueue();

    protected:
        Result onEvent (Event * event) override;
        Result onQuitEvent (Event * event);
        Result onSleepEvent (SleepEvent * event);

        virtual Result _startExecute ();

        NativeThreadId native_id;
        ThreadId id { detail::g_invalidThreadId };
        std::atomic_bool running { false };
        std::atomic_bool sleeping { false };
        EventQueue queue;
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

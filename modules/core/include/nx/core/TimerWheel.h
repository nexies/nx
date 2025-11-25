//
// Created by nexie on 16.10.2025.
//

#ifndef TIMER_WHEEL_H
#define TIMER_WHEEL_H

#include <iostream>
#include <thread>

#include <list>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <optional>

namespace nx {
namespace detail
{
    typedef std::function<void()> timer_callback_t;
    typedef int64_t timer_index_t;
    static constexpr timer_index_t invalid_timer = std::numeric_limits<timer_index_t>::lowest();
    inline static timer_index_t next_timer_index_for_thread ()
    {
        thread_local timer_index_t current = 0;
        do
        {
            current += 1;
        }
        while (current == invalid_timer);
        return current;
    }

    enum class TimerType
    {
        Normal,
        Coarse,
        Precise
    };

    template<TimerType _Type>
    struct base_timer_descriptor {};

    template<>
    struct base_timer_descriptor<TimerType::Normal>
    {
        static constexpr TimerType type = TimerType::Normal;
        using duration_type = std::chrono::milliseconds;
        using clock_type = std::chrono::steady_clock;
        // using time_point_type = std::chrono::time_point<typename clock_type, typename duration_type>;
    };

    template<>
    struct base_timer_descriptor<TimerType::Coarse>
    {
        static constexpr TimerType type = TimerType::Coarse;
        using duration_type = std::chrono::seconds;
        using clock_type = std::chrono::steady_clock;
        // using time_point_type = std::chrono::time_point<clock_type, duration_type>;
    };

    template<>
    struct base_timer_descriptor<TimerType::Precise>
    {
        static constexpr TimerType type = TimerType::Precise;
        using duration_type = std::chrono::microseconds;
        using clock_type = std::chrono::high_resolution_clock;
        // using time_point_type = std::chrono::time_point<typename clock_type, typename duration_type>;
    };

    template <TimerType _Type>
    struct base_timer_t : public base_timer_descriptor<_Type>
    {
        using desc = base_timer_descriptor<_Type>;
        using duration_type = typename desc::duration_type;

        inline void fire () { cb(); }
        inline bool is_repeating () const { return interval.has_value(); }

        base_timer_t (timer_index_t p_idx, timer_callback_t p_cb) :
            index(p_idx), cb(std::move(p_cb))
        {}

        base_timer_t(timer_index_t p_idx, timer_callback_t p_cb, duration_type p_interval) :
            index(p_idx), cb(std::move(p_cb)), interval(p_interval)
        {

        }

    public:
        timer_index_t index {};
        timer_callback_t cb;
        std::optional<duration_type> interval;
    };

    typedef base_timer_t<TimerType::Normal> Timer;
    typedef base_timer_t<TimerType::Precise> PreciseTimer;
    typedef base_timer_t<TimerType::Coarse> CoarseTimer;

    template <TimerType _type, size_t _Slots_count = 512>
    class base_timer_wheel_t
    {
        class timer : public base_timer_t<_type>
        {
        public:
            timer (timer_index_t p_idx, timer_callback_t p_cb, size_t p_overlaps) :
                base_timer_t<_type>(p_idx, p_cb), overlaps(p_overlaps)
            {}
            timer (timer_index_t p_idx, timer_callback_t p_cb, typename base_timer_t<_type>::duration_type interval, size_t p_overlaps) :
                        base_timer_t<_type>(p_idx, p_cb, interval), overlaps(p_overlaps)
            {}

            size_t overlaps { 0 };
        };

    public:
        using duration = typename timer::duration_type;
        using wheel_slot = std::list<timer>;
        using wheel = std::vector<wheel_slot>;
        static constexpr size_t slots_count = _Slots_count;
        duration rotation_duration () const { return _tick * slots_count; }
    private:
        std::unordered_set<timer_index_t> _active_timers;
        std::unordered_set<timer_index_t> _canceled_timers;
        wheel _wheel;
        size_t _current_slot { 0 };
        duration _tick;
        std::chrono::nanoseconds _carry = std::chrono::nanoseconds(0);

        bool m_push_timer (timer_index_t index, duration p_dur, timer_callback_t p_cb, bool p_repeating);
        bool m_cancel_timer (timer_index_t p_idx);
        wheel_slot & m_get_next_slot ();
        void m_single_tick ();
    public:
        explicit base_timer_wheel_t (duration p_tick = duration(1)) :
            _wheel(slots_count), _current_slot(0), _tick(p_tick)
        { }

        timer_index_t add_periodic (duration p_dur, timer_callback_t p_cb);
        timer_index_t add_singleshot (duration p_dur, timer_callback_t p_cb);
        bool cancel_timer (timer_index_t p_idx);

        bool has_active_timer(timer_index_t p_idx);
        bool has_canceled_timer(timer_index_t p_idx);

        template<typename _Duration>
        void process_time_elapsed (_Duration p_dur);

        void exec ();
    };

    template <enum TimerType _type, size_t _Slots_count>
    inline bool base_timer_wheel_t<_type, _Slots_count>::m_push_timer(timer_index_t index, duration p_dur,
        timer_callback_t p_cb, bool p_repeating)
    {
        // if (has_active_timer(index))
        //     return false;
        // size_t overlaps = (p_dur / rotation_duration());
        //
        // size_t slot = (_current_slot + p_dur  / _tick - overlaps * slots_count) % slots_count;
        //
        // if (!p_repeating)
        //     _wheel[slot].push_back({index, std::move(p_cb), overlaps});
        // else
        //     _wheel[slot].push_back({index, std::move(p_cb), p_dur, overlaps});
        //
        // _active_timers.insert(index);
        // return true;

        try
        {
            if (has_active_timer(index))
                return false;

            const size_t ticks_total = static_cast<size_t>(p_dur / _tick);
            const size_t overlaps   = ticks_total / slots_count;
            const size_t offset     = ticks_total % slots_count;
            const size_t slot       = (_current_slot + offset) % slots_count;

            if (!p_repeating)
                _wheel[slot].push_back({index, std::move(p_cb), overlaps});
            else
                _wheel[slot].push_back({index, std::move(p_cb), p_dur, overlaps});
            _active_timers.insert(index);
            return true;
        }
        catch ( ... )
        {
            std::cerr << "Timer wheel process exception" << std::endl;
            return false;
        }
    }

    template <enum TimerType _type, size_t _Slots_count>
    inline bool base_timer_wheel_t<_type, _Slots_count>::m_cancel_timer(timer_index_t p_idx)
    {
        if (!has_active_timer(p_idx))
            return false;

        _canceled_timers.insert(p_idx);
        return true;
    }

    template <enum TimerType _type, size_t _Slots_count>
    inline typename base_timer_wheel_t<_type, _Slots_count>::wheel_slot& base_timer_wheel_t<_type, _Slots_count>::m_get_next_slot()
    {
        _current_slot = (_current_slot + 1) % slots_count;
        return _wheel[_current_slot];
        // auto& slot = _wheel[_current_slot];
        // _current_slot = (_current_slot + 1) % slots_count;
        // return slot;
    }

    template <enum TimerType _type, size_t _Slots_count>
    void base_timer_wheel_t<_type, _Slots_count>::m_single_tick()
    {
        auto & slot = m_get_next_slot();
        for (auto it = slot.begin(); it != slot.end();)
        {
#define erase_iterator(it) \
it = slot.erase(it); \
if (it == slot.end()) \
break;

            if (has_canceled_timer(it->index))
            {
                _canceled_timers.erase(it->index);
                _active_timers.erase(it->index);
                // it = slot.erase(it);
                // if (it == slot.end())
                // break;
                erase_iterator(it);
                continue;
            }

            if (it->overlaps > 0)
            {
                // if (--it->overlaps > 0) {
                // ++it;
                // continue;
                // }
                --it->overlaps;
                ++it;
                continue;
            }
            it->cb();


            if (it->is_repeating())
            {
                _active_timers.erase(it->index);
                m_push_timer(it->index, it->interval.value(), std::move(it->cb), true);
                erase_iterator(it);
                continue;
            }

            _active_timers.erase(it->index);
            erase_iterator(it);

#undef erase_iterator
        }
    }

    template <enum TimerType _type, size_t _Slots_count>
    timer_index_t base_timer_wheel_t<_type, _Slots_count>::add_periodic(duration p_dur, timer_callback_t p_cb)
    {
        auto idx = next_timer_index_for_thread();
        return m_push_timer(idx, p_dur, std::move(p_cb), true) ? idx : invalid_timer;
    }

    template <enum TimerType _type, size_t _Slots_count>
    timer_index_t base_timer_wheel_t<_type, _Slots_count>::add_singleshot(duration p_dur, timer_callback_t p_cb)
    {
        auto idx = next_timer_index_for_thread();
        return m_push_timer(idx, p_dur, std::move(p_cb), false) ? idx : invalid_timer;
    }

    template <enum TimerType _type, size_t _Slots_count>
    bool base_timer_wheel_t<_type, _Slots_count>::cancel_timer(timer_index_t p_idx)
    {
        return m_cancel_timer(p_idx);
    }

    template <enum TimerType _type, size_t _Slots_count>
    inline bool base_timer_wheel_t<_type, _Slots_count>::has_active_timer(timer_index_t p_idx)
    {
        auto it = _active_timers.find(p_idx);
        return it != _active_timers.end();
    }

    template <enum TimerType _type, size_t _Slots_count>
    inline bool base_timer_wheel_t<_type, _Slots_count>::has_canceled_timer(timer_index_t p_idx)
    {
        auto it = _canceled_timers.find(p_idx);
        return it != _canceled_timers.end();
    }

    template <enum TimerType _type, size_t _Slots_count>
    template <typename _Duration>
    void base_timer_wheel_t<_type, _Slots_count>::process_time_elapsed(_Duration p_dur)
    {
        // size_t ticks = p_dur / _tick;
        // for (auto i = 0; i < ticks; i++)
        //     m_single_tick();

        _carry += p_dur;

        // Сколько полных тиков накопилось?
        const size_t ticks = static_cast<size_t>(_carry / _tick);
        if (ticks == 0)
            return;

        // Прокрутить колесо на ticks позиций
        for (size_t i = 0; i < ticks; ++i)
            m_single_tick();

        // Оставить дробной части столько, чтобы она была < _tick
        _carry -= ticks * _tick;
    }

    template <enum TimerType _type, size_t _Slots_count>
    void base_timer_wheel_t<_type, _Slots_count>::exec()
    {
        using clock = typename timer::clock_type;  // для Normal это пока system_clock — см. ниже
        auto last = clock::now();

        while (true) {
            auto now = clock::now();
            auto elapsed = now - last;

            if (elapsed >= _tick) {
                // Сколько тиков прошло?
                const auto ticks = static_cast<size_t>(elapsed / _tick);
                process_time_elapsed(ticks * _tick);   // прокрутить ровно ticks тиков
                last += ticks * _tick;                 // оставить «хвост» < _tick
            }

            std::this_thread::sleep_until(last + _tick);
        }
    }

}

    template <size_t _Slots_count = 1024>
    using TimerWheel = detail::base_timer_wheel_t<detail::TimerType::Normal, _Slots_count>;

    template <size_t _Slots_count = 1024>
    using CoarseTimerWheel = detail::base_timer_wheel_t<detail::TimerType::Coarse, _Slots_count>;

    template<size_t _Slots_count = 1024>
    using PreciseTimerWheel = detail::base_timer_wheel_t<detail::TimerType::Precise, _Slots_count>;

} // namespace nx

#endif //TIMER_WHEEL_H

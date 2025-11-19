//
// Created by nexie on 09.11.2025.
//

/**
 *  nx::Invoker
 *
 *  Шаблонная структура для хранения вызова функции/метода класса с аргументами (отложенный вызов)
 *  Хранит указатель на функцию в nx::Functor, аргументы для вызова в std::tuple.
 *
**/

#ifndef INVOKER_HPP
#define INVOKER_HPP

#include "app/Functor.hpp"

namespace nx {

    namespace detail
    {
        class InvokerBase {
        protected:
            virtual void invoke_impl () = 0;
            virtual ~InvokerBase() = default;
        public:
            void invoke () { invoke_impl(); }
        };
    }
    //
    // template<typename Invocable, bool FromNxFunctor, typename... Args>
    // class Invoker;

    /**
    https://static.wikia.nocookie.net/dota2_gamepedia/images/2/22/Invoker_Guide_Header.png/revision/latest/scale-to-width-down/550?cb=20160811091903
    **/
    template<typename... Args>
    class Invoker: public detail::InvokerBase
    {
        using FunctorInputType = detail::FunctorInput<Args...>;
        FunctorPtr _functor;
        std::tuple<Args...> _args;

        void invoke_impl () override
        {
            auto input = static_cast<FunctorInputType *>(_functor.get());
            std::apply(*input, _args);
        }
    public:
        explicit Invoker(FunctorPtr functor, Args... args) :
            _functor(std::move(functor)),
            _args(args...)
        {}

        template<typename Ret>
        explicit Invoker(Ret(*f)(Args...), Args... args) : Invoker(nx::make_functor(f), args...) {}

        template<typename Class, typename Ret>
        explicit Invoker(Class* c, Ret(Class::*f)(Args...), Args... args) : Invoker(nx::make_functor(c, f), args...) {}

        template<typename Class, typename Ret>
        explicit Invoker(Class const * c, Ret(Class::*f)(Args...) const, Args... args) : Invoker(nx::make_functor(c, f), args...) {}
    };

    typedef std::shared_ptr<detail::InvokerBase> InvokerPtr;

    template<typename... Args>
    InvokerPtr make_invoker(Args... args) { return InvokerPtr(new Invoker(args...)); }


    //     template <typename Ret, typename... Args, typename... Params>
    //     Invoker05 (Ret(*)(Args...), Params&&...) -> Invoker05<Ret(Args...), Args...>;
    //
    //     template <typename Class, typename Ret, typename... Args, typename... Params>
    //     Invoker05 (Class *, Ret(Class::*)(Args...), Params&&...) -> Invoker05<Class, Ret(Class*, Args...), Args...>;
    //
    //     template <typename Class, typename Ret, typename... Args, typename... Params>
    //     Invoker05 (Class*, Ret(Class::*)(Args...)const, Params&&...) -> Invoker05<Class const, Ret(Class const*, Args...), Args...>;
    //

#if 0
    template<typename... Args>
    class Invoker : public InvokerBase {
        using Func = Functor<Class, Ret, Args...>;
        Func _functor;
        std::tuple<Args...> _args;

        void invoke_impl() override {
            std::apply(_functor, std::move(_args));
        }
    public:
        explicit Invoker(Class *cls, typename Func::Func func, Args&&... args) :
            _functor(cls, func),
            _args(std::forward<Args>(args)...)
        {}
    };

    template<typename Ret, typename... Args>
    class Invoker<void, Ret, Args...> : public InvokerBase {
        using Func = Functor<void, Ret, Args...>;
        Func _functor;
        std::tuple<Args...> _args;

        void invoke_impl() override {
            std::apply(_functor, std::move(_args));
        }

    public:
        explicit Invoker(typename Func::Func func, Args&&... args) :
            _functor(func),
            _args(std::forward<Args>(args)...)
        {}
    };

    template<typename Ret, typename... Args, typename... Params>
    Invoker(Ret(*)(Args...), Params&&...) -> Invoker<void, Ret(Args...), Args...>;

    template <typename Class, typename Ret, typename... Args, typename... Params>
    Invoker (Class *, Ret(Class::*)(Args...), Params&&...) -> Invoker<Class, Ret(Class*, Args...), Args...>;

    template <typename Class, typename Ret, typename... Args, typename... Params>
    Invoker (Class*, Ret(Class::*)(Args...)const, Params&&...) -> Invoker<Class const, Ret(Class const*, Args...), Args...>;

    typedef std::shared_ptr<InvokerBase> InvokerPtr;
    template<typename... Args>
    InvokerPtr make_invoker(Args&&... args) { return std::shared_ptr<InvokerBase>(std::forward<Args>(args)...); }
#endif
}


#endif //INVOKER_HPP

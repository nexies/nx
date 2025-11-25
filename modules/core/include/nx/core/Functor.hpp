//
// Created by nexie on 10.11.2025.
//

/**
 *  nx::Functor
 *
 *  Шаблонная оболочка над указателем на функцию или методом класса.
 *  При создании оболочки вокруг метода класса также принимает указатель на объект класса,
 *  таким образом не требуя передачи указателя (this) в качестве первого параметра при вызове.
 *
 *  Обобщают вызов функции/метода в базовом FunctionInput, сводя таким образом шаблон только к типу
 *  аргументов функции/метода.
 *
 *  Имеет спецификации для хранения:
 *      1) Указателя на функцию
 *      2) Указателя на метод класса
 *      3) Указателя на константный метод класса
 *
 *  Нужно дополнить:
 *      - Спецификацию для хранения объектов типа lambda
 *      - Спецификацию для хранения указателя на noexcept функцию
 *      - Спецификацию для хранения указателя на noexcept метод класса
 *      - Спецификацию для хранения указателя на const noexcept метод класса
**/

#ifndef FUNCTOR2_H
#define FUNCTOR2_H
#include <utility>
#include <memory>

namespace nx
{
    namespace detail
    {
        class FunctorBase {
        public:
            virtual ~FunctorBase() = default;
            virtual size_t hash() = 0;
        };

        template<typename ... Args>
        class FunctorInput : public FunctorBase
        {
        protected:
            virtual void noreturn_call_impl (Args&&... args) = 0;
        public:
            template<typename... Params>
            static void noreturn_call (FunctorInput * self, Params&&... params)
            {
                self->noreturn_call_impl (std::forward<Params>(params)...);
            }

            template<typename... Params>
            void operator() (Params&&... params)
            {
                noreturn_call_impl (std::forward<Args>(params)...);
            }
        };

        template<typename Class, typename Ret, typename... Args>
        using ClassMethod = Ret(Class::*)(Args...);

        template<typename Class, typename Ret, typename... Args>
        using ConstClassMethod = Ret(Class::*const)(Args...)const;

        template<typename Ret, typename... Args>
        using Function = Ret(Args...);

        template<typename Ret, typename... Args>
        using FunctionRef = Ret(*)(Args...);

    }

    template<class Class, typename Ret, bool Const, typename ... Args>
    class Functor;

    /// For storing class methods
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, false, Args...>: public detail::FunctorInput<Args...>
    {
    public:
        using Signature = Ret(Class::*)(Args...);
        using ReturnType = Ret;
        using ArgsTuple = std::tuple<Args...>;
    private:
        Signature _func;
        Class * _cls;

    protected:
        void noreturn_call_impl (Args&&... args) override
        {
            operator()(std::forward<Args>(args)...);
        }

    public:
        Functor(Class * cls, Signature func) :
            _cls(cls), _func(func) {}

        ReturnType operator() (Args... args)
        {
            return (_cls->*_func)(args...);
        }

        size_t hash() override {
            auto s1 = std::hash<void *>{}(reinterpret_cast<void *>(_cls));
            auto s2 = std::hash<void *>{}(reinterpret_cast<void *>(_func));
            return s1 + (s2 << 1);
        }
    };

    /// For storing const class methods
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, true, Args...> : public detail::FunctorInput<Args...>
    {
    public:
        using Signature = Ret(Class::*)(Args...) const;
        using ReturnType = Ret;
        using ArgsTuple = std::tuple<Args...>;
    private:
        Signature _func;
        Class* _cls;
    protected:
        void noreturn_call_impl(Args&&... args) override { operator()(std::forward<Args>(args)...); }
    public:
        Functor(Class* cls, Signature func) : _cls(cls), _func(func) {}

        ReturnType operator()(Args... args) const
        {
            return (_cls->*_func)(args...);
        }

        size_t hash() override {
            auto s1 = std::hash<void *>{}(reinterpret_cast<void *>(_cls));
            auto s2 = std::hash<void *>{}(reinterpret_cast<void *>(_func));
            return s1 + (s2 << 1);
        }
    };

    /// For storing functions
    template <typename Ret, typename... Args>
    class Functor<void, Ret, false, Args...> : public detail::FunctorInput<Args...>
    {
    public:
        using Signature = Ret(*)(Args...);
        using ReturnType = Ret;
        using ArgsTuple = std::tuple<Args...>;
    private:
        Signature _func;

    protected:
        void noreturn_call_impl(Args&&... args) override { operator()(std::forward<Args>(args)...); }

    public:
        explicit Functor(Signature func) : _func(func) { }

        ReturnType operator()(Args... args)
        {
            return _func(args...);
        }

        size_t hash() override {
            return std::hash<void *>{}(reinterpret_cast<void *>(_func));
        }
    };

    template<typename Ret, typename... Args>
    Functor(Ret(*)(Args...)) -> Functor<void, Ret, false, Args...>;

    template <typename Class, typename Ret, typename... Args>
    Functor(Class*, Ret (Class::*)(Args...)) -> Functor<Class, Ret, false, Args...>;

    template <typename Class, typename Ret, typename... Args>
    Functor(Class const*, Ret(Class::*)(Args...)const) -> Functor<Class const, Ret, true, Args...>;


    typedef std::shared_ptr<detail::FunctorBase> FunctorPtr;
    template<typename... Args>
    FunctorPtr make_functor(Args&& ... args) {
        return FunctorPtr(new Functor(std::forward<Args>(args)...));
    }
}


#endif //FUNCTOR2_H

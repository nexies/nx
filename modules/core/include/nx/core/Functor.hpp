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
    }

    template<class Class, typename Ret, bool Const, bool Owning, typename ... Args>
    class Functor;

    /// For storing class methods
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, false, false, Args...>: public detail::FunctorInput<Args...>
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
            // C-style cast to get around functions and invokables cast restrictions
            auto s1 = std::hash<void *>{}((void *)(_cls));
            auto s2 = std::hash<void *>{}((void *)(_func));
            return s1 + (s2 << 1);
        }
    };

    /// For storing const class methods
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, true, false, Args...> : public detail::FunctorInput<Args...>
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
            // C-style cast to get around functions and invokables cast restrictions
            auto s1 = std::hash<void *>{}((void *)(_cls));
            auto s2 = std::hash<void *>{}((void *)(_func));
            return s1 + (s2 << 1);
        }
    };

        /// For storing class methods (owning)
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, false, true, Args...>: public detail::FunctorInput<Args...>
    {
    public:
        using Signature = Ret(Class::*)(Args...);
        using ReturnType = Ret;
        using ArgsTuple = std::tuple<Args...>;
    private:
        Signature _func;
        Class _cls;

    protected:
        void noreturn_call_impl (Args&&... args) override
        {
            operator()(std::forward<Args>(args)...);
        }

    public:
        Functor(Class && cls, Signature func) :
            _cls(std::forward<Class>(cls)), _func(func) {}

        // invokable
        Functor(Class && cls) :
            Functor(std::forward<Class>(cls), &Class::operator()) {}

        ReturnType operator() (Args... args)
        {
            return (_cls.*_func)(args...);
        }

        size_t hash() override {
            // C-style cast to get around functions and invokables cast restrictions
            auto s1 = std::hash<void *>{}((void *)(&_cls));
            auto s2 = std::hash<void *>{}((void *)(_func));
            return s1 + (s2 << 1);
        }
    };

    /// For storing const class methods (owning)
    template<class Class, typename Ret, typename ... Args>
    class Functor<Class, Ret, true, true, Args...> : public detail::FunctorInput<Args...>
    {
    public:
        using Signature = Ret(Class::*)(Args...) const;
        using ReturnType = Ret;
        using ArgsTuple = std::tuple<Args...>;
    private:
        Signature _func;
        Class _cls;
    protected:
        void noreturn_call_impl(Args&&... args) override { operator()(std::forward<Args>(args)...); }
    public:
        Functor(Class && cls, Signature func) : _cls(std::forward<Class>(cls)), _func(func) {}

        // invokable
        Functor(Class && cls) :
            Functor(std::forward<Class>(cls), &Class::operator()) {}

        ReturnType operator()(Args... args) const
        {
            return (_cls.*_func)(args...);
        }

        size_t hash() override {
            // C-style cast to get around functions and invokables cast restrictions
            auto s1 = std::hash<void *>{}((void *)(&_cls));
            auto s2 = std::hash<void *>{}((void *)(_func));
            return s1 + (s2 << 1);
        }
    };

    /// For storing functions
    template <typename Ret, typename... Args>
    class Functor<void, Ret, false, false, Args...> : public detail::FunctorInput<Args...>
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
            // C-style cast to get around functions and invokables cast restrictions
            return std::hash<void *>{}((void *)(_func));
        }
    };

    template<typename Ret, typename... Args>
    Functor(Ret(*)(Args...)) -> Functor<void, Ret, false, false, Args...>;

    template <typename Class, typename Ret, typename... Args>
    Functor(Class*, Ret (Class::*)(Args...)) -> Functor<Class, Ret, false, false, Args...>;

    template <typename Class, typename Ret, typename... Args>
    Functor(Class const*, Ret(Class::*)(Args...)const) -> Functor<Class const, Ret, true, false, Args...>;

    // template <typename Class, typename Ret, typename... Args>
    // Functor(Class)


    typedef std::shared_ptr<detail::FunctorBase> FunctorPtr;
    template<typename... Args>
    FunctorPtr make_functor(Args&& ... args) {
        return FunctorPtr(new Functor(std::forward<Args>(args)...));
    }
}


#endif //FUNCTOR2_H

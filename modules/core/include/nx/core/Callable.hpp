//
// Created by nexie on 03.12.2025.
//

#ifndef CALLABLE_HPP
#define CALLABLE_HPP

#include "Tuple.hpp"

#include "nx/experimental/reflect"

namespace nx
{
    namespace detail
    {
        /// FunctionDescriptor has to detect:
        ///     1) return value (all)
        ///     2) parameters types (all)
        ///     3) noexcept (all)
        ///     4) const (member functions)
        ///     5) volatile (member functions)
        ///     6) const volatile (member functions)
        ///     7) variadic ... (all) ??
        ///     8) extern "C" (all) ??
        ///     9) extern "C++" (all) ??
        ///     10) ref & (member functions) ??
        ///     11) ref && (member functions) ??

        namespace function
        {
        }

        template <typename Signature>
        struct FunctionDescriptor;

        template <typename Ret, typename... Args>
        struct FunctionDescriptor<Ret(Args...)>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Args...> InvokeArguments;
            typedef Ret (Signature)(Args...);
            typedef Ret (*Pointer)(Args...);

            static constexpr bool Noexcept = false;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = false;
        };

        template <typename Ret, typename... Args>
        struct FunctionDescriptor<Ret(*)(Args...)>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Args...> InvokeArguments;
            typedef Ret (Signature)(Args...);
            typedef Ret (*Pointer)(Args...);

            static constexpr bool Noexcept = false;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = false;
        };

        template <typename Ret, typename... Args>
        struct FunctionDescriptor<Ret(Args...) noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Args...> InvokeArguments;
            typedef Ret (Signature)(Args...) noexcept;
            typedef Ret (*Pointer)(Args...) noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = false;
        };

        template <typename Ret, typename... Args>
        struct FunctionDescriptor<Ret(*)(Args...) noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Args...> InvokeArguments;
            typedef Ret (Signature)(Args...) noexcept;
            typedef Ret (*Pointer)(Args...) noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = false;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...)>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...);
            typedef Ret (Class::*Pointer)(Args...);

            static constexpr bool Noexcept = false;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) noexcept;
            typedef Ret (Class::*Pointer)(Args...) noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = false;
            static constexpr bool Volatile = false;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) const>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) const;
            typedef Ret (Class::*Pointer)(Args...) const;

            static constexpr bool Noexcept = false;
            static constexpr bool Const = true;
            static constexpr bool Volatile = false;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) const noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) const noexcept;
            typedef Ret (Class::*Pointer)(Args...) const noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = true;
            static constexpr bool Volatile = false;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) volatile>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...);
            typedef Ret (Class::*Pointer)(Args...);

            static constexpr bool Noexcept = false;
            static constexpr bool Const = false;
            static constexpr bool Volatile = true;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) volatile noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) noexcept;
            typedef Ret (Class::*Pointer)(Args...) noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = false;
            static constexpr bool Volatile = true;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) const volatile>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) const;
            typedef Ret (Class::*Pointer)(Args...) const;

            static constexpr bool Noexcept = false;
            static constexpr bool Const = true;
            static constexpr bool Volatile = true;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionDescriptor<Ret(Class::*)(Args...) const volatile noexcept>
        {
            typedef Ret Return;
            typedef Tuple<Args...> Arguments;
            typedef Tuple<Class*, Args...> InvokeArguments;
            typedef Ret (Class::*Signature)(Args...) const noexcept;
            typedef Ret (Class::*Pointer)(Args...) const noexcept;

            static constexpr bool Noexcept = true;
            static constexpr bool Const = true;
            static constexpr bool Volatile = true;

            static constexpr bool Member = true;
            typedef Class MemberOf;
        };

        template <typename Invokable, typename ArgTuple>
        class FunctionRef;

        template <typename Invokable, typename... Args>
        class FunctionRef<Invokable, Tuple<Args...>> : public FunctionDescriptor<Invokable>
        {
            using Desc = detail::FunctionDescriptor<Invokable>;
            using Pointer = typename Desc::Pointer;
            Pointer f;
        public:
            using Return = typename Desc::Return;

        public:
            constexpr bool isNoexcept() const { return Desc::Noexcept; }
            constexpr bool isConst() const { return Desc::Const; }
            constexpr bool isVolatile() const { return Desc::Volatile; }
            constexpr bool isMember() const { return Desc::Member; }
            constexpr std::string_view getTypeName() const { return reflect::type_name<Invokable>(); };

            // constexpr std::string_view getPointerType () const;

            FunctionRef(Pointer f) : f(f)
            {
            }

            decltype(auto) operator ()(Args... args)
            {
                // return Tuple<Args...>{std::forward<Args>(args)...}.apply_to(f);
                return Tuple<Args...>{args...}.apply_to(f);
            }
        };

        template <typename Invokable>
        FunctionRef(Invokable) -> FunctionRef<
            Invokable,
            typename detail::FunctionDescriptor<Invokable>::InvokeArguments
        >;

        // template<typename Invokable, bool isStrong, bool isMember, typename ArgsTuple>
        // class Callable;
        //
        // template<typename Invokable, bool isStrongT, typename ... Args>
        // class Callable<Invokable, isStrongT, false, Tuple<Args...>> : public FunctionRef<Invokable, Tuple<Args...>>
        // {
        //     using Base = FunctionRef<Invokable, Tuple<Args...>>;
        // public:
        //     Callable(typename Base::Pointer f) : Base(f) {}
        //     constexpr bool isStrong() const { return false; }
        // };
        //
        // template<typename Invokable, typename ... Args>
        // class Callable<Invokable, false, true, Tuple<Args...>> :
        //     protected FunctionRef<Invokable, Tuple<typename FunctionDescriptor<Invokable>::MemberOf *, Args...>>
        // {
        //     using Class = typename FunctionDescriptor<Invokable>::MemberOf;
        //     using Base = FunctionRef<Invokable, Tuple<Class*, Args...>>;
        //
        //     Class & o;
        // public:
        //     Callable(std::reference_wrapper<Class> o, typename Base::Pointer f) : Base(f), o(o.get()) {}
        //     constexpr bool isStrong() const { return false; }
        //
        //     typename Base::Return operator () (Args... args)
        //     {
        //         return Tuple{&o, args...}.apply_to(this->f);
        //     }
        // };
        //
        // template<typename Invokable, typename ... Args>
        // class Callable<Invokable, true, true, Tuple<Args...>> :
        // protected FunctionRef<Invokable, Tuple<typename FunctionDescriptor<Invokable>::MemberOf *, Args...>>
        // {
        //     using Class = typename FunctionDescriptor<Invokable>::MemberOf;
        //     using Base = FunctionRef<Invokable, Tuple<Class*, Args...>>;
        //
        //     Class o;
        // public:
        //     Callable(Class && o, typename Base::Pointer f) : Base(f), o(std::move(o)) {}
        //     constexpr bool isStrong() const { return true; }
        //
        //     typename Base::Return operator () (Args... args)
        //     {
        //         return Tuple{&o, args...}.apply_to(this->f);
        //     }
        // };
        //
        // template<typename Invokable, std::enable_if<not FunctionDescriptor<Invokable>::Member>>
        // Callable(Invokable)
        //     -> Callable<Invokable, false, false, typename FunctionDescriptor<Invokable>::InvokeArguments>;
        //
        // template<typename Invokable, std::enable_if<FunctionDescriptor<Invokable>::Member>>
        // Callable (std::reference_wrapper<typename FunctionDescriptor<Invokable>::MemberOf>,
        //          typename FunctionDescriptor<Invokable>::Pointer)
        //     -> Callable<Invokable, false, true, typename FunctionDescriptor<Invokable>::Arguments>;
        //
        // template<typename Invokable, std::enable_if<FunctionDescriptor<Invokable>::Member>>
        // Callable (typename FunctionDescriptor<Invokable>::MemberOf &&,
        //         typename FunctionDescriptor<Invokable>::Pointer)
        //     -> Callable<Invokable, true, true, typename FunctionDescriptor<Invokable>::Arguments>;

        template<typename FunctionRefT, bool isMember, typename ...>
        class Callable;

        // function callable
        template<typename FunctionRefT>
        class Callable<FunctionRefT, false> : public FunctionRefT
        {
        public:
            Callable(FunctionRefT && f) : FunctionRefT(std::move(f)) {}
        };

        template<typename Function, std::enable_if<not FunctionDescriptor<Function>::Member, int>::type = 0>
        Callable(Function) -> Callable<FunctionRef<Function, typename FunctionDescriptor<Function>::InvokeArguments>, false>;

        // member function as reference
        template<typename FunctionRefT, typename Class, typename ... CallArgs>
        class Callable<FunctionRefT, true, Class, Tuple<CallArgs...>>
        {
            using Return = typename FunctionRefT::Return;
            using ClassDecay = std::remove_reference_t<Class>;

            Class o;
            FunctionRefT f;
        public:
            Callable(std::reference_wrapper<ClassDecay> o, FunctionRefT && f) : o(o.get()), f(std::move(f)) {}
            Callable(ClassDecay && o, FunctionRefT && f) : o(std::move(o)), f(std::move(f)) {}
            Callable(ClassDecay && o) : Callable(std::forward<ClassDecay>(o), &ClassDecay::operator()) {}

            Return operator () (CallArgs ... args)
            {
                return Tuple<ClassDecay*, CallArgs...>{&o, args...}.apply_to(f);
            }
        };

        template<typename Function, std::enable_if<FunctionDescriptor<Function>::Member, int>::type = 0>
        Callable(
            std::reference_wrapper<typename FunctionDescriptor<Function>::MemberOf>,
            Function)
        -> Callable<
            FunctionRef<Function, typename FunctionDescriptor<Function>::InvokeArguments>,
            true,
            typename FunctionDescriptor<Function>::MemberOf &,
            typename FunctionDescriptor<Function>::Arguments>;

        template<typename Function, std::enable_if<FunctionDescriptor<Function>::Member, int>::type = 0>
        Callable(
        typename FunctionDescriptor<Function>::MemberOf &&,
        Function)
        -> Callable<
        FunctionRef<Function, typename FunctionDescriptor<Function>::InvokeArguments>,
        true,
        typename FunctionDescriptor<Function>::MemberOf,
        typename FunctionDescriptor<Function>::Arguments>;

        template<typename Invokable, std::enable_if<FunctionDescriptor<decltype(&Invokable::operator())>::Member, int>::type = 0>
        Callable(Invokable &&)
        -> Callable<
        FunctionRef<decltype(&Invokable::operator()), typename FunctionDescriptor<decltype(&Invokable::operator())>::InvokeArguments>,
        true,
        Invokable,
        typename FunctionDescriptor<decltype(&Invokable::operator())>::Arguments>;
    }



}

#endif //CALLABLE_HPP

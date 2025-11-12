//
// Created by nexie on 10.11.2025.
//

#ifndef FUNCTOR2_H
#define FUNCTOR2_H
#include <tuple>

namespace nx
{
    namespace experimental
    {
        namespace detail
        {
            class FunctorBase {};
            template<typename ... Args>
            class FunctorInput : public FunctorBase {
                virtual void noreturn_call_impl (Args... args) = 0;
            public:
                virtual ~FunctorInput() = default;
                void  noreturn_call (Args... args) {
                    noreturn_call_impl (args...);
                }
            };

            enum FunctorType
            {
                Function,
                FunctionReference,
                ClassMethodReference,
                ConstClassMethodReference,
                ClassMethod,
                ConstClassMethod,
            };

            template<FunctorType Type, typename ... Args>
            struct FunctionDescriptor;

            template<typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::Function, Ret, Args...>
            {
                using Return = Ret;

                using Signature = Ret(Args...);
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Args...);
                using ApplyArguments = std::tuple<Args...>;
            };

            template<typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::FunctionReference, Ret, Args...>
            {
                using Return = Ret;

                using Signature = Ret(*)(Args...);
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Args...);
                using ApplyArguments = std::tuple<Args...>;
            };

            template<typename Class, typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::ClassMethod, Class, Ret, Args...>
            {
                using Instance = Class;
                using FromInstance = Class &&;

                using Return = Ret;

                using Signature = Ret(Class::*)(Args...);
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Class *, Args...);
                using ApplyArguments = std::tuple<Class*, Args...>;
            };

            template<typename Class, typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::ConstClassMethod, Class, Ret, Args...>
            {
                using Instance = Class;
                using FromInstance = Class &&;

                using Return = Ret;

                using Signature = Ret(Class::*)(Args...) const;
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Class * const, Args...);
                using ApplyArguments = std::tuple<Class* const, Args...>;
            };

            template<typename Class, typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::ClassMethodReference, Class, Ret, Args...>
            {
                using Instance = Class &;
                using FromInstance = Class &;

                using Return = Ret;

                using Signature = Ret(Class::*)(Args...);
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Class *, Args...);
                using ApplyArguments = std::tuple<Class*, Args...>;
            };

            template<typename Class, typename Ret, typename... Args>
            struct FunctionDescriptor<FunctorType::ConstClassMethodReference, Class, Ret, Args...>
            {
                using Instance = Class &;
                using FromInstance = Class &;

                using Return = Ret;

                using Signature = Ret(Class::*)(Args...) const;
                using Arguments = std::tuple<Args...>;

                using ApplySignature = Ret(*)(Class * const, Args...);
                using ApplyArguments = std::tuple<Class* const, Args...>;
            };
        }

        template<detail::FunctorType Type, typename... Args>
        // class Functor : public detail::FunctorInput<detail::FunctionDescriptor<Type,  Args...>::Arguments::> {};
    }
}

#endif //FUNCTOR2_H

// //
// // Created by nexie on 09.11.2025.
// //
//
// #ifndef INVOKER_HPP
// #define INVOKER_HPP
//
// #include "Functor.hpp"
// namespace nx {
//
//     class InvokerBase {
//     protected:
//         virtual void m_invoke_impl () const = 0;
//         virtual ~InvokerBase() = default;
//     public:
//         virtual void invoke () const {
//             m_invoke_impl();
//         }
//     };
//
//     template<class Signature, typename... Args>
//     class Invoker05 : public InvokerBase {
//         using Func = Functor<Signature, Args...>;
//         Func _functor;
//         std::tuple<Args...> _args;
//
//         void m_invoke_impl () const override {
//             _functor.call(std::move(_args));
//         }
//
//     public:
//         explicit Invoker05(func function, Args&&... args) :
//             _function(std::move(function)),
//             _args(std::forward<Args>(args)...)
//         {}
//     };
//
//     template<class Class, class Signature, typename... Args>
//     class Invoker05 : public InvokerBase {
//         using desc = ::nx::FunctionDescriptor<Signature>;
//         using func = typename desc::function_type;
//         using result = typename desc::result_type;
//
//         func _function;
//         Class * _inst;
//         std::tuple<Args...> _args;
//
//         void m_invoke_impl () const override {
//             std::apply(_function, _inst, std::move(_args));
//         }
//
//     public:
//         explicit Invoker05(Class * inst, func function, Args&&... args) :
//             _inst(inst),
//             _function(std::move(function)),
//             _args(std::forward<Args>(args)...)
//         {}
//     };
//
//
//     template<typename Invocable>
//     class Invoker05<Invocable, void> : public InvokerBase{
//         Invocable _func;
//         void m_invoke_impl () const override {
//             _func();
//         }
//     public:
//         explicit Invoker05(Invocable func) :
//             _func(std::move(func))
//         {}
//
//     };
//
//     template <typename Ret, typename... Args, typename... Params>
//     Invoker05 (Ret(*)(Args...), Params&&...) -> Invoker05<Ret(Args...), Args...>;
//
//     template <typename Class, typename Ret, typename... Args, typename... Params>
//     Invoker05 (Class *, Ret(Class::*)(Args...), Params&&...) -> Invoker05<Class, Ret(Class*, Args...), Args...>;
//
//     template <typename Class, typename Ret, typename... Args, typename... Params>
//     Invoker05 (Class*, Ret(Class::*)(Args...)const, Params&&...) -> Invoker05<Class const, Ret(Class const*, Args...), Args...>;
//
//     template<typename Invocable>
//     Invoker05 (Invocable) -> Invoker05<Invocable, void>;
//
//     using InvokerPtr = std::shared_ptr<InvokerBase>;
//     template<typename... Args>
//     InvokerPtr make_invoker(Args&&... args) { return std::shared_ptr<InvokerBase>( new Invoker05(std::forward<Args>(args)...)); }
//
// }
//
// #endif //INVOKER_HPP

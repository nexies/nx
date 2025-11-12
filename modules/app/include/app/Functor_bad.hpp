// //
// // Created by nexie on 09.11.2025.
// //
//
// #ifndef FUNCTOR_HPP
// #define FUNCTOR_HPP
//
// #include <functional>
// #include <tuple>
//
// namespace nx
// {
//     namespace bad {
//
//         template<typename Signature>
//         using Function = std::function<Signature>;
//
//         namespace detail {
//             template<typename Signature>
//             struct std_function_descriptor {
//                 using function_type = Function<Signature>;
//                 using result_type = typename function_type::result_type;
//             };
//         }
//         template<typename Signature>
//         using FunctionDescriptor = detail::std_function_descriptor<Signature>;
//
//         struct FunctorBase {};
//
//         template<typename... Args>
//         struct FunctorInput : public FunctorBase {
//             virtual ~FunctorInput() = default;
//             // virtual void call_noreturn (Args... args) = 0;
//             using argument_tuple_type = std::tuple<Args...>;
//         };
//
//         // Functor containing class methods
//         template<typename Class, typename Signature, typename... Args>
//         class Functor_bad: public FunctorInput<Args...> {
//             using desc = FunctionDescriptor<Signature>;
//         public:
//             using function_type = typename desc::function_type;
//             using return_type  = typename desc::result_type;
//         private:
//             Class * _inst;
//             function_type _func;
//         public:
//             Functor_bad(Class * inst, function_type func) :
//                 _inst(inst), _func(func)
//             {};
//
//             template<typename ... Params>
//             return_type operator() (Params&&... params) {
//                 // return std::apply(_func, std::make_tuple(_inst, args...));
//                 // return std::apply(_func, std::make_tuple(_inst, args...));
//                 return (_inst->*_func)(std::forward<Args>(params)...);
//             }
//             return_type operator() (Args&&... args) const {
//                 // return std::apply(_func, std::make_tuple(_inst, args...));
//             }
//
//             // void call_noreturn (Args... args) override {
//             //     std::apply(_func, std::make_tuple(_inst, args...));
//             // }
//         };
//
//         // Functor containing functions
//         template<typename Signature, typename... Args>
//         class Functor_bad<void, Signature, Args...>: public FunctorInput<Args...> {
//             using desc = FunctionDescriptor<Signature>;
//         public:
//             using Func = typename desc::function_type;
//             using Ret  = typename desc::result_type;
//
//         private:
//             Func _func;
//         public:
//             explicit Functor_bad(Func func) :
//                 _func (std::move(func))
//             {}
//             template<typename... Params>
//             Ret operator() (Params&&... params) {
//                 return _func(std::forward<Args>(params)...);
//             }
//
//             // void call_noreturn (Args... args) override {
//             //     std::apply(_func, std::make_tuple(args...));
//             // }
//         };
//
//         template<typename Ret, typename... Args>
//         Functor_bad(Ret(*)(Args...)) -> Functor_bad<void, Ret(Args...), Args...>;
//
//         template<typename Class, typename Ret, typename... Args>
//         Functor_bad(Class *, Ret(Class::*)(Args...)) -> Functor_bad<Class, Ret(Class*, Args...), Args...>;
//
//         template<typename Class, typename Ret, typename... Args>
//         Functor_bad(Class const *, Ret(Class::*)(Args...)const) -> Functor_bad<Class const, Ret(Class const*, Args...), Args...>;
//
//         typedef std::shared_ptr<FunctorBase> FunctorPtr;
//         template<typename... Args>
//         FunctorPtr make_functor(Args&& ... args) { return std::shared_ptr<FunctorBase> (new Functor_bad(std::forward<Args>(args)...)); }
//
//     }
// }
// #endif //FUNCTOR_HPP

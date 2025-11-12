//
// Created by nexie on 11.11.2025.
//

#ifndef OVERLOAD_H
#define OVERLOAD_H

#include "app/Functor.hpp"

namespace nx
{
    using namespace detail;
    //
    // template<typename Signature, typename Class, typename Ret, typename... Args>
    // ClassMethod<Class, Ret, Args...> overload (Signature func)
    // {
    //     return static_cast<ClassMethod<Class, Ret, Args...>>(func);
    // }
    //
    // template<typename Class, typename Ret, typename... Args, typename... Args2>
    // ClassMethod<Class, Ret, Args2...> overload(ClassMethod<Class, Ret, Args...>) -> overload<ClassMethod<Class, Ret, Args...>, Ret, Args2...>;
}

#endif //OVERLOAD_H
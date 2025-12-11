//
// Created by nexie on 09.11.2025.
//

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>
#include <sys/stat.h>

#include "Result.hpp"
#include <nx/core/Exception.hpp>

namespace nx {
    template <class Class>
    class Singleton : public Class {
        Singleton () = default;
        Singleton(const Singleton &) = delete;
        Singleton(Singleton &&) = delete;
        Singleton & operator = (const Singleton &) = delete;
        Singleton & operator = (Singleton &&) = delete;

        static Singleton & S_this_instance ()
        {
            static Singleton inst {};
            return inst;
        }

        std::unique_ptr<Class> _class_instance { nullptr };
    public:
        template<typename... Args>
        static Result Init(Args&&... args);
        static Result Free();
        static Class & Instance ();
    };

    template<class Class>
    template<typename ... Args>
    Result Singleton<Class>::Init(Args &&...args) {
        auto & self = S_this_instance();
        if (self._class_instance)
            return Result::Err("Singleton::Init(): Singleton instance already exists");

        self._class_instance = std::make_unique<Class>(std::forward<Args>(args)...);
        if (!self._class_instance)
            return Result::Err("Singleton::Init(): Error creating instance");

        return Result::Ok();
    }

    template<class Class>
    Result Singleton<Class>::Free() {
        auto & self = S_this_instance();
        if (!self._class_instance)
            return Result::Err("Singleton::Free(): Instance is empty");
        self._class_instance.reset(nullptr);
        return Result::Ok();
    }

    template<class Class>
    Class & Singleton<Class>::Instance() {
        auto & self = S_this_instance();
        if (!self._class_instance)
            if (!Init()) throw nx::Exception("Instance is not initialized and does not have a default constructor");

        return *S_this_instance()._class_instance.get();
    }
}

#endif //SINGLETON_HPP

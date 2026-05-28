//
// Created by nexie on 4/7/2026.
//

#ifndef NX_COMMON_SINGLETON_HPP
#define NX_COMMON_SINGLETON_HPP

#include <nx/common/types/errors.hpp>
#include <nx/make_unique.hpp>

#include "result.hpp"

namespace nx
{
    namespace detail
    {
        template <typename Class>
        class meyers_singleton
        {
        public:
            using value_type = Class;
            using reference = value_type&;

            static reference
            get_instance ()
            {
                static value_type instance;
                return instance;
            }

            NX_DISABLE_COPY(meyers_singleton);
            NX_DISABLE_MOVE(meyers_singleton);
        protected:
            meyers_singleton () = default;
            ~meyers_singleton () = default;
        };


        template <typename Class>
        class smart_singleton
        {
        public:
            using value_type = Class;
            using reference = value_type&;
            using ptr_type  = std::unique_ptr<value_type>;

        private:
            // C++11: static member of class template is defined in the header
            // below the class body (inline static is C++17).
            static ptr_type instance_;

        public:

            template<typename ... Args>
            static void
            init (Args && ... args)
            {
                if (instance_ != nullptr)
                    throw std::runtime_error ("singleton instance is already initialized");

                instance_ = nx::make_unique<Class>(std::forward<Args>(args)...);
                if (!instance_)
                {
                    throw std::runtime_error ("error on initialization");
                }
            }

            template<typename ... Args>
            static void
            reset (Args && ... args)
            {
                instance_.reset(nullptr);
                instance_ = nx::make_unique<Class>(std::forward<Args>(args)...);
                if (!instance_)
                    throw std::runtime_error ("error on reset");
            }

            static void
            free ()
            {
                instance_.reset(nullptr);
            }

            NX_NODISCARD reference
            instance ()
            {
                if (!instance_)
                    throw std::runtime_error ("instance is not initialized");
                return *instance_;
            }

        };

        // Out-of-line definition for the static member of the class template.
        // Allowed in headers for templates; only one definition is generated per TU.
        template<typename Class>
        typename smart_singleton<Class>::ptr_type smart_singleton<Class>::instance_ { nullptr };

    }

    template<typename Class>
    class enable_singleton_from_this : public detail::smart_singleton<Class>
    {};
}

#endif //NX_COMMON_SINGLETON_HPP

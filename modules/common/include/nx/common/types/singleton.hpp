//
// Created by nexie on 4/7/2026.
//

#ifndef NX_COMMON_SINGLETON_HPP
#define NX_COMMON_SINGLETON_HPP

#include <nx/common/types/errors.hpp>

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
            inline static ptr_type instance_ { nullptr };
        public:

            template<typename ... Args>
            static void
            init (Args && ... args)
            {
                if (instance_ != nullptr)
                    throw std::runtime_error ("singleton instance is already initialized");

                instance_ = std::make_unique<Class>(std::forward<Args>(args)...);
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
                instance_ = std::make_unique<Class>(std::forward<Args>(args)...);
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
    }

    template<typename Class>
    class enable_singleton_from_this : public detail::smart_singleton<Class>
    {};
}

#endif //NX_COMMON_SINGLETON_HPP
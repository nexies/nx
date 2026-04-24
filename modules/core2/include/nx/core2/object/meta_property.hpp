//
// Created by nexie on 4/20/2026.
//

#ifndef NX_CORE2_META_PROPERTY_HPP
#define NX_CORE2_META_PROPERTY_HPP

#include <functional>
#include <any>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/meta_error.hpp>
#include <fmt/format.h>

namespace nx::core
{
    class object;

    namespace detail
    {
        using property_index_t = std::size_t;

        template<typename Object, typename PropType>
        struct property_descriptor
        {
            // static_assert (std::is_base_of<object, Object>::value, "NX_PROPERTY can be only used with nx::core::object and it's inheritors");
            using object_type = Object;
            using type = PropType;

            using member_ptr = type object_type::*;
            using member_getter = std::function<type(object_type *)>;
            using member_setter = std::function<void(object_type *, const type &)>;
            using member_notif  = std::function<void(object_type *)>;
            using member_reset  = std::function<void(object_type *)>;

            static constexpr auto cast_to_any = [] (const type & value) -> std::any
            {
                return std::any(value);
            };

            static constexpr auto cast_from_any = [] (const std::any & value) -> result<type>
            {
                if (value.type() == typeid(type))
                    return std::any_cast<type>(value);
                return err::property_error(fmt::format("Cannot cast type {} to type {}", value.type().name(), typeid(type).name()));
            };

            bool registered { false };
        };

        template<typename Object>
        struct meta_property_descriptor
        {
            static_assert (std::is_base_of<object, Object>::value, "NX_PROPERTY can be only used with nx::core::object and it's inheritors");
            using object_type = Object;

            using meta_getter = std::function<std::any (object_type *)>;
            using meta_setter = std::function<nx::result<void> (object_type *, const std::any &)>;
            using meta_notif  = std::function<void(object_type *)>;
            using meta_reset  = std::function<void(object_type *)>;

            const char * name  { "undefied" };
            meta_getter getter { nullptr };
            meta_setter setter { nullptr };
            meta_notif  notif  { nullptr };
            meta_reset  reset  { nullptr };
        };

        template<typename Object>
        struct meta_property_registry
        {
            using meta_desc_t = meta_property_descriptor<Object>;

            template <typename Type>
            using desc_t = property_descriptor<Object, Type>;

        private:
            std::unordered_map<std::string_view, meta_desc_t> meta_descriptors_;

        public:
            template<typename Type>
            auto
            register_object_property(
                std::string_view name,
                typename desc_t<Type>::member_ptr ptr = nullptr,
                typename desc_t<Type>::member_getter getter = nullptr,
                typename desc_t<Type>::member_setter setter = nullptr,
                typename desc_t<Type>::member_notif notif = nullptr,
                typename desc_t<Type>::member_reset reset = nullptr
            ) -> desc_t<Type>
            {
                if (auto it = meta_descriptors_.find(name); it != meta_descriptors_.end())
                {
                    throw err::property_error(fmt::format("property {} already exists", name));
                }

                // desc_t<Type> desc;
                // desc.name = name;
                meta_desc_t meta_desc;

                if (getter != nullptr)
                {
                    meta_desc.getter = [g = std::move(getter)] (Object * obj) -> std::any { return std::invoke(g, obj); };
                }

                if (setter != nullptr)
                {
                    meta_desc.setter = [s = std::move(setter)] (Object * obj, const std::any & value) -> result<void>
                    {
                        auto v = desc_t<Type>::cast_from_any(value);
                        if (v)
                        {
                            std::invoke(s, obj, v.value());
                            return {};
                        }
                        return v.error();
                    };
                }

                if (notif != nullptr)
                {
                    meta_desc.notif = [n = std::move(notif)] (Object * obj) -> void
                    {
                        std::invoke(n, obj);
                    };
                }

                if (reset != nullptr)
                {
                    meta_desc.reset = [r = std::move(reset)] (Object * obj) -> void
                    {
                        std::invoke(r, obj);
                    };
                }

                if (meta_desc.getter == nullptr && ptr != nullptr)
                {
                    meta_desc.getter = [p = ptr] (Object * obj) -> std::any { return obj->*p; };
                }

                if (meta_desc.setter == nullptr && ptr != nullptr)
                {
                    meta_desc.setter = [p = ptr] (Object * obj, const std::any & value) -> result<void>
                    {
                        auto v = desc_t<Type>::cast_from_any(value);
                        if (v)
                        {
                            obj->*p = v.value();
                            return {};
                        }
                        else
                            return v.error();
                    };
                }

                auto res = meta_descriptors_.try_emplace(name, meta_desc);

                return { true };
            }

            NX_NODISCARD result<std::any>
            static_get (std::string_view name, Object * obj)
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                {
                    return err::property_error(fmt::format("property {} is not registered", name));
                }

                if (it->second.getter == nullptr)
                {
                    return err::property_error(fmt::format("property {} does not have a getter", name));
                }

                return it->second.getter(obj);
            }

            result<void>
            static_set (std::string_view name, Object * obj, const std::any & value)
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                {
                    return err::property_error(fmt::format("property {} is not registered", name));
                }

                if (it->second.setter == nullptr)
                {
                    return err::property_error(fmt::format("property {} does not have a setter", name));
                }

                return it->second.setter(obj, value);
            }

            result<void>
            static_reset (std::string_view name, Object * obj)
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return err::property_error(fmt::format("property {} is not registered", name));
                if (it->second.reset == nullptr)
                    return err::property_error(fmt::format("property {} does not have a reset function", name));
                it->second.reset(obj);
                return {};
            }

            result<void>
            static_notify (std::string_view name, Object * obj)
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return err::property_error(fmt::format("property {} is not registered", name));
                if (it->second.notif == nullptr)
                    return err::property_error(fmt::format("property {} does not have a notification signal", name));
                it->second.notif(obj);
                return {};
            }

            NX_NODISCARD size_t
            property_count() const
            {
                return meta_descriptors_.size();
            }

            NX_NODISCARD bool
            has_property(std::string_view name) const
            {
                auto it = meta_descriptors_.find(name);
                return it != meta_descriptors_.end();
            }

            NX_NODISCARD bool
            has_getter(std::string_view name) const
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return false;
                return it->second.getter != nullptr;
            }

            NX_NODISCARD bool
            has_setter(std::string_view name) const
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return false;
                return it->second.setter != nullptr;
            }

            NX_NODISCARD bool
            has_reset(std::string_view name) const
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return false;
                return it->second.reset != nullptr;
            }

            NX_NODISCARD bool
            has_notify(std::string_view name) const
            {
                auto it = meta_descriptors_.find(name);
                if (it == meta_descriptors_.end())
                    return false;
                return it->second.notif != nullptr;
            }


        };

    }
}

#endif //NX_CORE2_META_PROPERTY_HPP

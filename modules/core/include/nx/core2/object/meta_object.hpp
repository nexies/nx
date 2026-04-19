//
// Created by nexie on 19.04.2026.
//

#ifndef NX_CORE2_META_OBJECT_HPP
#define NX_CORE2_META_OBJECT_HPP

#include <any>
#include <functional>
#include <type_traits>

#include <nx/core2/object/object.hpp>

#include "fmt/bundled/base.h"

namespace nx::core {

    namespace detail {
        template<typename ClassType, typename MemberType>
        struct property_functions {
            using member_reference = MemberType ClassType:: *;

            using property_getter = std::function<MemberType(ClassType *)>;
            using property_setter = std::function<void(ClassType *, const MemberType &)>;
            using property_notifier = std::function<void(ClassType *)>;
            using property_resetter = std::function<void(ClassType *)>;

            using type = MemberType;
        };

        template<typename ClassType>
        struct vague_property_descriptor {
            using property_getter = std::function<std::any (ClassType *)>;
            using property_setter = std::function<void(ClassType *, const std::any &)>;
            using property_notifier = std::function<void(ClassType *)>;
            using property_resetter = std::function<void(ClassType *)>;

            std::string name;
            property_getter getter { nullptr };
            property_setter setter { nullptr };
            property_notifier notifier { nullptr };
            property_resetter resetter { nullptr };

        };
    }

    template<typename Object>
    class meta_object {
        static_assert(std::is_base_of<Object, Object>::value, "type Object must be derived from nx::core::object");

        template<typename PropertyType>
        using fn = detail::property_functions<Object, PropertyType>;

        using vds = detail::vague_property_descriptor<Object>;

        static inline std::unordered_map<uint32_t, vds> vague_descriptors_;
        static inline uint32_t current_property_id_ = 0;
    public:
        using object_type = Object;

        template <typename t>
        static uint32_t
        register_object_property (
            std::string_view name,
            typename fn<t>::member_reference reference,
            typename fn<t>::property_getter getter = nullptr,
            typename fn<t>::property_setter setter = nullptr,
            typename fn<t>::property_notifier notifier = nullptr,
            typename fn<t>::property_resetter resetter = nullptr
        ) {
            auto id = current_property_id_++;
            vds v;

            if (getter != nullptr) {
                v.getter = [g = getter] (object_type * object) -> std::any {
                    return std::invoke(g, object);
                };
            }

            if (setter != nullptr) {
                v.setter = [s = setter] (object_type * object, const std::any & value) -> void {
                    return std::invoke(s, object, std::any_cast<t>(value));
                };
            }

            if (notifier != nullptr) {
                v.notifier = [n = notifier] (object_type * object) -> void {
                    return std::invoke(n, object);
                };
            }

            if (resetter != nullptr) {
                v.resetter = [r = resetter] (object_type * object) -> void {
                    return std::invoke(r, object);
                };
            }

            v.name = name;

            vague_descriptors_.try_emplace(id, std::move(v));

            return id;
        }


        static std::any
        static_get_property (object_type & object, uint32_t property_id) {
            return vague_descriptors_.at(property_id).getter(&object);
        }

        static
        void static_set_property (object_type & object, uint32_t property_id, std::any value) {
            vague_descriptors_.at(property_id).setter(&object, value);
        }

    };
}

#endif //NX_CORE2_META_OBJECT_HPP

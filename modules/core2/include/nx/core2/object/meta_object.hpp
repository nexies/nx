//
// Created by nexie on 4/20/2026.
//

#ifndef NX_CORE2_META_OBJECT_HPP
#define NX_CORE2_META_OBJECT_HPP

#include <nx/core2/object/meta_property.hpp>

namespace nx::core
{
    class object;

    namespace detail
    {
        template <typename Object>
        class meta_object
        {
            using meta_property_descriptor = meta_property_descriptor<Object>;
            using meta_property_registry = meta_property_registry<Object>;

            std::string_view object_class_name_ = "undefined";
            meta_property_registry property_registry_;

        public:
            meta_object() = default;

            meta_object(std::string_view object_class_name) :
                object_class_name_(object_class_name)
            { }

            NX_NODISCARD meta_property_registry&
            property_registry() { return property_registry_; }

            NX_NODISCARD std::string_view
            class_name () const noexcept { return object_class_name_; }
        };
    }
}

#endif //NX_CORE2_META_OBJECT_HPP

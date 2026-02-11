//
// Created by nexie on 29.12.2025.
//

#ifndef NXTBOT_OBJECT_DEFS_HPP
#define NXTBOT_OBJECT_DEFS_HPP

#include <type_traits>
#include <nx/core/detail/property_defs_cxx20.hpp>

#define _nx_this_method_owner_type(Type) \
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>;

#define _nx_object_check_type(Class) \
    static_assert(std::is_base_of_v<::nx::Object, Class>, "NX_OBJECT macro can be used only for nx::Object inheritors");

#define _nx_object_decl(Class) \
    _nx_enable_properties(Class) \

    // _nx_object_check_type(Class) \

#define NX_OBJECT(Class) \
    _nx_object_decl(Class) \


#endif //NXTBOT_OBJECT_DEFS_HPP
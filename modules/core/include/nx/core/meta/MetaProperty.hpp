//
// Created by nexie on 17.03.2026.
//

#ifndef NX_METAPROPERTY_HPP
#define NX_METAPROPERTY_HPP
#include <any>
#include <functional>
#include <optional>

#include <nx/core/Object.hpp>

namespace nx
{
    using MetaPropertyNameGetter = std::function<std::string_view (Object *)>;
    using MetaPropertySetter = std::function<void (Object *, std::any)>;
    using MetaPropertyGetter = std::function<std::any (Object *)>;
    using MetaPropertyReSetter = std::function<void (Object *)>;
    using MetaPropertyCommentGetter = std::function<std::string_view (Object *)>;


    class MetaPropertyDescriptor
    {
        std::optional<MetaPropertyCommentGetter> nameGetter = std::nullopt;
        std::optional<MetaPropertySetter> getter = std::nullopt;
        std::optional<MetaPropertySetter> setter = std::nullopt;
        std::optional<MetaPropertyGetter> resetter = std::nullopt;
        std::optional<MetaPropertyCommentGetter> commentGetter = std::nullopt;
    };
}

#endif //NX_METAPROPERTY_HPP
//
// Created by nexie on 19.12.2025.
//

#ifndef NX_PROPERTY_IMPL_HPP
#define NX_PROPERTY_IMPL_HPP

#include <functional>
#include <iostream>

#ifndef __nx_cxx_20_property &&
#warning "nx meta property system is only avalable with std::c++20 standard or higher"
#elif !defined __nx_cxx_20_reflect
#warning "nx meta property system requires reflect extension"
#else

#define NX_CXX20_PROPERTY_IMPL

#include <nx/experimental/reflect>
#include <nx/core/Signal.hpp>

namespace nx::detail
{
    template<typename>
    struct member_pointer_type;

    template<typename Class, typename T>
    struct member_pointer_type<T Class::*> {
        using type = T;
    };

    template<typename MP>
    using member_pointer_type_t = typename member_pointer_type<MP>::type;

    template<typename T>
    concept DataMemberPointer = requires {
        typename member_pointer_type<T>;
    };

    struct base_property_tag {};

    template<typename HostType>
    struct property_tag : public base_property_tag
    {
        using host_type = HostType;
    };

    template<typename Type>
    concept HasPropertyTag = std::is_base_of_v<base_property_tag, Type>;

    template<typename T>
    concept NotReference = !std::is_reference<T>::value;

    template<typename Prop>
    concept NamedProperty =
        requires { {Prop::name} -> std::convertible_to<std::string_view>; };

    template<typename Prop>
    concept TypedProperty =
        requires { typename Prop::type; requires NotReference<typename Prop::type>; };

    template<typename Prop>
    concept Property = HasPropertyTag<Prop> && TypedProperty<Prop> and NamedProperty<Prop>;

    template<typename Type>
    constexpr bool isProperty() { if constexpr (Property<Type>) return true; else return false; }

    template<Property Prop>
    constexpr std::string_view propertyName()
    {
        return Prop::name;
    }

    template<Property Prop>
    constexpr std::string_view propertyTypeName()
    {
        return reflect::type_name<typename Prop::type>();
    }

    template<Property Prop>
    constexpr std::string_view propertyHostTypeName()
    {
        return reflect::type_name<typename Prop::host_type> ();
    }

    template<typename Prop>
    concept PropertyWithDefault =
        Property<Prop> and
        requires { { Prop::default_value } -> std::convertible_to<typename Prop::type>; };

    template<Property Prop>
    constexpr bool propertyHasDefaultValue() { if constexpr (PropertyWithDefault<Prop>) return true; else return false; }

    template<PropertyWithDefault Prop>
    constexpr auto propertyDefaultValue() { return Prop::default_value; }

    template<typename Prop>
    concept PropertyWithSetter =
    Property<Prop> and
    requires(typename Prop::host_type && obj,
             const typename Prop::type& value) // аргумент, который мы будем пробовать передать
    {
        { std::invoke(Prop::set, obj, value) };
    };

    template<Property Prop>
    constexpr bool propertyHasSetter() { return PropertyWithSetter<Prop>; }

    template<PropertyWithSetter Prop>
    constexpr std::string_view propertySetterTypeName() { return typeid(Prop::set).name(); }

    template<PropertyWithSetter Prop>
    auto callPropertySetter(typename Prop::host_type & host, const typename Prop::type & value )
    {
        return std::invoke(Prop::set, host, value);
    }

    template<PropertyWithSetter Prop>
    auto callPropertySetter(typename Prop::host_type * host, const typename Prop::type & value )
    {
        return std::invoke(Prop::set, host, value);
    }

    template<typename Prop>
    concept PropertyWithGetter =
        Property<Prop> and
        requires (typename Prop::host_type& obj) {
        { std::invoke(Prop::get, obj) }
            -> std::convertible_to<typename Prop::type>;
        };

    template<Property Prop>
    constexpr bool propertyHasGetter() { return PropertyWithGetter<Prop>; }

    template<PropertyWithGetter Prop>
    constexpr std::string_view propertyGetterTypeName() { return typeid(Prop::get).name(); }

    template<PropertyWithGetter Prop>
    auto callPropertyGetter(typename Prop::host_type & host)
    {
        return std::invoke(Prop::get, host);
    }

    template<PropertyWithGetter Prop>
    auto callPropertyGetter(typename Prop::host_type * host)
    {
        return std::invoke(Prop::get, host);
    }

    template <typename Prop>
    concept PropertyWithReset =
        Property<Prop> and
        requires (typename Prop::host_type& obj) {
        { std::invoke(Prop::reset, obj) };
    };

    template<Property Prop>
    constexpr bool propertyHasReset() { return PropertyWithReset<Prop>; }

    template<PropertyWithReset Prop>
    constexpr std::string_view propertyResetTypeName() { return typeid(Prop::reset).name(); }

    template<typename Prop>
    concept PropertyWithNotify =
        Property<Prop> and
        requires(typename Prop::host_type * obj, typename Prop::type value)
        {
            NX_MAKE_EMIT(Prop::notify, obj, value)
        };

    template<Property Prop>
    constexpr bool propertyHasNotify()  { return PropertyWithNotify<Prop>; }

    template<PropertyWithNotify Prop>
    constexpr std::string_view propertyNotifyTypeName () { return  typeid(Prop::notify).name(); }

    template<typename Prop>
    void DumpPropertyInfo (Prop & p)
    {
        constexpr auto type_name = reflect::type_name<Prop>();

        if constexpr (not HasPropertyTag<Prop>) {
            std::cerr << "Type \"" << type_name << "\" is missing a property_tag, e.g. is not a property" << std::endl;
            return;
        } else if constexpr (not NamedProperty<Prop>) {
            std::cerr << "Type \"" << type_name << "\" is missing a property name" << std::endl;
            return;
        } else if constexpr (not TypedProperty<Prop>) {
            std::cerr << "Type \"" << type_name << "\" is missing a property type" << std::endl;
        }
        else if constexpr (not Property<Prop>) {
            std::cerr << type_name << " is not a property" <<std::endl;
        } else {
            std::cerr << "Property \"" << type_name << "\" of class " << propertyHostTypeName<Prop>() << ": " << std::endl;
            std::cerr << "\tType: " << propertyTypeName<Prop>() << std::endl;
            std::cerr << "\tName: " << propertyName<Prop>() << std::endl;

            if constexpr (PropertyWithDefault<Prop>)
                std::cerr << "\tHas default value: true\n\t\tDefault value: " << propertyDefaultValue<Prop>() << std::endl;
            else
                std::cerr << "\tHas default value: false" << std::endl;

            if constexpr (PropertyWithGetter<Prop>)
                std::cerr << "\tHas getter: true\n\t\tGetter: " << propertyGetterTypeName<Prop>() << std::endl;
            else
                std::cerr << "\tHas getter: false" << std::endl;

            if constexpr (PropertyWithSetter<Prop>)
                std::cerr << "\tHas setter: true\n\t\tSetter: " << propertySetterTypeName<Prop>() << std::endl;
            else
                std::cerr << "\tHas setter: false" << std::endl;

            if constexpr (PropertyWithReset<Prop>)
                std::cerr << "\tHas reset: true\n\t\tReset: " << propertyResetTypeName<Prop>() << std::endl;
            else
                std::cerr << "\tHas reset: false" << std::endl;

            if constexpr (PropertyWithNotify<Prop>)
                std::cerr << "\tHas notify: true\n\t\tNotify: " << propertyNotifyTypeName<Prop>() << std::endl;
            else
                std::cerr << "\tHas notify: false" << std::endl;

        }
    }
}

#endif
#endif //NXTBOT_PROPERTY_IMPL_HPP
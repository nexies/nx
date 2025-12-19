//
// Created by nexie on 18.12.2025.
//

#include <iostream>
#include <any>

#include "../../../reflect-cpp/include/rfl/thirdparty/ctre.hpp"
#include "nx/experimental/reflect"
#include "spdlog/fmt/bundled/base.h"

namespace nx::detail
{
    template <class T, std::size_t Size>
    struct fixed_string
    {
        constexpr explicit(false) fixed_string(const T* str)
        {
            for (decltype(Size) i{}; i < Size; ++i) { data[i] = str[i]; }
            data[Size] = T();
        }

        [[nodiscard]] constexpr auto operator<=>(const fixed_string&) const = default;
        [[nodiscard]] constexpr explicit(false) operator std::string_view() const { return {std::data(data), Size}; }
        [[nodiscard]] constexpr auto size() const { return Size; }
        T data[Size + 1u];
    };

    template <class T, std::size_t Capacity, std::size_t Size = Capacity - 1>
    fixed_string(const T (&str)[Capacity]) -> fixed_string<T, Size>;
}

#include <nx/experimental/reflect>

template <typename T, std::size_t Size1, std::size_t Size2>
constexpr reflect::fixed_string<T, Size1 + Size2> fixed_string_concat(reflect::fixed_string<T, Size1> s1,
                                                                      reflect::fixed_string<T, Size2> s2)
{
    T buf[Size1 + Size2 + 1u];
    for (decltype(Size1) i{}; i < Size1; i = i + 1)
        buf[i] = s1.data[i];
    for (decltype(Size2) i{}; i < Size2; i = i + 1)
        buf[i + Size1] = s2.data[i];
    return reflect::fixed_string(buf);
}

struct base_property_desc_t
{
};

template <typename Class>
struct PropertyManager
{
    constexpr explicit(false) PropertyManager()
    {
    }

    template <reflect::fixed_string str>
    bool _hasProperty()
    {
        constexpr auto member_name = _propertyDescriptorName(str);
        if constexpr (reflect::has_member_name<Class, member_name>)
        {
            constexpr auto idx = reflect::index_of<member_name, Class>();
            using property_type = reflect::member_type<idx, Class>;
            if constexpr (std::is_base_of<base_property_desc_t, property_type>::value)
            {
                std::cerr << "has property " << std::string_view(str) << " which is valid" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "has property " << std::string_view(str) << " which is not valid" << std::endl;
                return false;
            }
        }
        return false;
    }

    template <typename T, std::size_t Size>
    constexpr auto _propertyDescriptorName(reflect::fixed_string<T, Size> name)
    {
        return fixed_string_concat(reflect::fixed_string{"m_property_descriptor_"}, name);
    }
};

#define __NX_GET_THIS_TYPE(TypeName) \
    using TypeName = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>

#define __NX_ENABLE_PROPERTY(Class) \
    auto & _PropertyManager () {\
        __NX_GET_THIS_TYPE(This); \
        static PropertyManager<This> man;\
        return man;\
    } \
    template<typename T, std::size_t Size> \
    constexpr bool HasProperty(constexpr reflect::fixed_string<T, Size> name) {\
        return this->_PropertyManager().template _hasProperty<name>(); \
    }

#define __nx_enable_properties(Class) \
    using This = Class;

#define NX_ENABLE_PROPERTIES(...) \
    __nx_enable_properties(__VA_ARGS__)

#define __NX_OBJECT(Class) \
    NX_ENABLE_PROPERTIES(Class)

#define NX_OBJECT(...) \
    __NX_OBJECT(__VA_ARGS__)

#include "nx/core/detail/property_impl.hpp"

struct Struct
{
    NX_ENABLE_PROPERTIES(Struct)

    int value;

    void setValue(const std::string& value)
    {
        std::cerr << "setValue(" << value << ");" << std::endl;
        float res;
        std::from_chars(value.data(), value.data() + value.size(), res);
        this->value = res;
    }

    std::string getValue() const
    {
        return std::to_string(value);
    }

    struct someProperty : nx::detail::property_tag<This>
    {
        /// name is MANDATORY
        static constexpr std::string name = "SomeProperty";
        /// type is MANDATORY
        using type = std::string;
        /// default_value is OPTIONAL
        static constexpr type default_value = "12";
        /// set is OPTIONAL
        static constexpr auto set = [] (host_type & obj, const std::string & val) {
            std::cerr << "Lambda set: " << val << std::endl;
            obj.value = std::stoi(val);
        };
        /// get is OPTIONAL
        static constexpr auto get = &host_type::getValue;
        /// reset is OPTIONAL
        // static constexpr auto reset = &host_type::resetValue;
    };
};

int main(int argc, char* argv[])
{
    using namespace nx::detail;

    std::cerr << std::boolalpha;

    Struct s{};

    Struct::someProperty p;
    Struct::someProperty::set(s, "123");
    // std::invoke(Struct::someProperty::set, s, "123");
    DumpPropertyInfo<Struct::someProperty>(p);

    return 0;
}

///
/// struct Creature
/// {
///     NX_ENABLE_PROPERTIES
///
///     NX_PROPERTY(TYPE float, NAME weight, READ weight, CONST, DEFAULT 40)
///         // 1) Creates member property descriptor - name "weight", type "float"
///         // 2) Creates member data field "const float m_weight" ???
///         // 3) Creates member getter - name "weight", const, return type "float"
///
///
///    Creature(float weight = propertyDefault<"weight">()) : m_weight(weight) {}
/// }
///
/// struct Person : public Creature
/// {
///     NX_PROPERTY(TYPE int, NAME id, READ id, WRITE setId)
///         // 1) Creates member property descriptor - name "id", type "int"
///         // 2) Creates member data field "int m_id";    ???
///         // 3) Creates member getter - name "id", const, return type std::string
///         // 4) Creates member setter - name "setId", argument const "std::string &", return void
///
///     struct Initials
///     {
///         std::string name;
///         std::string surname;
///     } m_initials;
///
///     std::string getName () const { return m_initials.name; }
///     std::string setName (std::string name) const { m_initials.name = name; }
///
///     NX_PROPERTY(TYPE std::string, NAME firstName, STORED false, MEMBER READ getName, MEMBER WRITE setName)
///         // 1) Creates member property descriptor - name "firstName", type "std::string"
///         // 2) Does not create getter - uses existing getter
///         // 3) Does not create setter - uses existing setter
///
///    Person() : Creature(80) {}
/// }
///
///     Person p;
///     p.setName("Phil");
///
///     p.property<"firstName">();  // calls Person::getName, returns "Phil";
///     p.setProperty<"firstName">("John") // calls Person::setName
///     p.property<"weight">(); // calls Creature::weight, returns 80
///     p.propertyDefault<"firstName">() // Undefined or returns {} ???
///
///
///     template<>
///     void setProperty<fixed_string{"firstName"}>(nx::const_reference_wrapper<std::string> new_value) const
///     {
///         this->setName()
///     }

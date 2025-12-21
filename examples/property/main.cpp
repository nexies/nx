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
#include <nx/core/Object.hpp>

struct Struct : public nx::Object
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

    NX_SIGNAL(valueChanged, std::string)

    struct someProperty : nx::detail::property_tag<This>
    {
        /// name is MANDATORY
        static constexpr std::string name = "SomeProperty";
        /// type is MANDATORY
        using type = std::string;
        /// default_value is OPTIONAL
        // static constexpr type default_value = "12";
        /// set is OPTIONAL
        static constexpr auto set = [] (host_type & obj, const std::string & val) {
            std::cerr << "Lambda set: " << val << std::endl;
            obj.value = std::stoi(val);
        };
        /// get is OPTIONAL
        // static constexpr auto get = &host_type::getValue;
        /// reset is OPTIONAL
        // static constexpr auto reset = [] (host_type & obj) {
        //     set(obj, default_value);
        // };
        /// notify is OPTIONAL
        static constexpr auto notify = &host_type::valueChanged;
    };
};


// #include <nx/macro/logic.hpp>
// #define NX_MAX_NUMBER 100
#include <nx/macro/numeric.hpp>
// #include <boost/preprocessor/while.hpp>
// #include <nx/macro/repeat/iterate.hpp>
#include <nx/macro/repeating/while.hpp>
#include "nx/macro/numeric/sum.hpp"
#include "nx/macro/numeric/sub.hpp"
#include "nx/macro/numeric/compare.hpp"

int main(int argc, char* argv[])
{
// #define _cond(a, b) _nx_bool(a)
// #define _op(a, b) _nx_dec(a), _nx_inc(b)
// #define _res(a, b) b

// #define _nx_add(a, b) _nx_while_0(_cond, _op, _res, 2, 2)

// #define _mul_cond(a, c) _nx_bool(a)
// #define _mul_op(a, b) _nx_dec(a), _nx_add(a, b)

// #define _nx_mul(a, b) _nx_while_0(_mul_cond, _mul_op, _res, a, b)

    NX_NUMERIC_SUM(0, 0);
    // _nx_numeric_sub(10, 4);
    // _nx_numeric_eq(64, 64);
   // std::cout << _nx_mul(1, 2) << std::endl;

// #define DECORATOR_0(data) { data,
// #define DECORATOR_1(data) data,
// #define DECORATOR_2(data) data }
// #define DECORATOR(c, data) NX_CONCAT(DECORATOR_, c)(data)
// #define ITERATOR(n, data) data
//
//     enum Enum {
//         E1,
//         E2,
//         E3
//     };
//
//     Enum vec [] = NX_DECORATED_ITERATE(ITERATOR, DECORATOR, E3, E2, E1);

    NX_INC(10);


    using namespace nx::detail;

    std::cerr << std::boolalpha;

    Struct s{};

    Struct::someProperty p;
    Struct::someProperty::set(s, "123");
    // std::invoke(Struct::someProperty::set, s, "123");
    DumpPropertyInfo<Struct::someProperty>(p);

    return 0;
}



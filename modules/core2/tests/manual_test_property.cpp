//
// Created by nexie on 4/20/2026.
//

#include <iostream>
#include <nx/core2/object/meta_property.hpp>
#include <nx/core2/object/object.hpp>

struct obj : public nx::core::object
{
    int value = 0;

    int get_value() const { return value; }

    void set_value (int value) { this->value = value; }
};

int main ()
{
    nx::core::detail::meta_property_registry<obj> reg;
    auto desc = reg.register_object_property<int>("value", &obj::value, &obj::get_value, &obj::set_value);

    obj o;
    auto set_res = reg.static_set("value", &o, 100.0);

    if (!set_res)
    {
        nx::explain(set_res.error());
    }
    else
        std::cerr << "value set: " << o.get_value() << std::endl;
    return 0;
}
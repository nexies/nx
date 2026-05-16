//
// Created by green on 5/1/2026.
//

#include <iostream>
#include <nx/core2/object.hpp>

class obj : public nx::core::object {

    float temp_;
    public:
    NX_OBJECT(obj)


public:
    NX_SIGNAL(changed)

    NX_SIGNAL(temp_changed)
    NX_PROPERTY(temp, MEMBER temp_, WRITE, READ, NOTIFY temp_changed, RESET, DEFAULT 100)
};


int main () {

    auto & reg = obj::static_meta_object().property_registry();

    obj o;

    nx::core::connect(&o, &obj::temp_changed, &o,
        [&] { std::cerr << "Temperature changed: " << o.get_temp() << std::endl; });

    reg.static_set("temp", &o, float(123));
    o.set_temp(32132);

    return 0;
}
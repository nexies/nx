//
// Created by nexie on 19.04.2026.
//

#include <iostream>
#include <ostream>
#include <nx/core2/object/meta_object.hpp>

struct obj {

    uint64_t id;

    int get_id () const { return id; }

    void set_id (uint64_t id) { this->id = id; }

    struct property_id_t {
        static constexpr auto name = "id";
        using type = uint64_t;
        const static inline uint64_t index =
            nx::core::meta_object<obj>::register_object_property<type>(
        name,
        &obj::id,
        &obj::get_id,
        &obj::set_id,
        nullptr,
        nullptr
        );

    };

};

using obj_meta = nx::core::meta_object<obj>;

auto idx =

// auto idx2 = obj_meta::register_object_property<int>(
//         "id",
//         &obj::id,
//         &obj::get_id,
//         &obj::set_id,
//         nullptr,
//         nullptr
// );

int main () {
    // auto i = &obj::id;

    std::cerr << idx << std::endl;

    obj o;
    o.id = 100;

    obj_meta::static_set_property(o, idx, uint64_t(123));
    auto p = obj_meta::static_get_property(o, idx);
    std::cerr << p.type().name() << std::endl;
    std::cerr << std::any_cast<uint64_t>(p) << std::endl;;
}
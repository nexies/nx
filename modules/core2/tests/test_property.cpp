//
// Created by nexie on 4/20/2026.
//

#include <catch2/catch_test_macros.hpp>
#include <nx/core2/object.hpp>


#include <any>
#include <string>
#include <string_view>


namespace nx::core
{
    // Если object уже объявлен/определён в проекте — этот блок убрать.
    // Здесь оставлен только как напоминание.
}

namespace
{
    class test_object : public nx::core::object
    {
    public:
        int value {0};
        int reset_count {0};
        int notify_count {0};
        std::string text {"init"};

        int get_value() const
        {
            return value;
        }

        void set_value(const int& v)
        {
            value = v;
        }

        void reset_value()
        {
            value = -1;
            ++reset_count;
        }

        NX_SIGNAL(notify_value)

        std::string get_text() const
        {
            return text;
        }

        void set_text(const std::string& s)
        {
            text = s;
        }
    };
}

TEST_CASE("meta_property_registry registers property by member pointer", "[meta_property][register][pointer]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto desc = registry.register_object_property<int>("value", &test_object::value);

    REQUIRE(desc.registered);
    REQUIRE(registry.property_count() == 1);
    REQUIRE(registry.has_property("value"));
    REQUIRE(registry.has_getter("value"));
    REQUIRE(registry.has_setter("value"));
    REQUIRE_FALSE(registry.has_reset("value"));
    REQUIRE_FALSE(registry.has_notify("value"));
}

TEST_CASE("meta_property_registry static_get returns value for member pointer property", "[meta_property][get]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    registry.register_object_property<int>("value", &test_object::value);

    test_object obj;
    obj.value = 123;

    auto result = registry.static_get("value", &obj);

    REQUIRE(result);
    REQUIRE(result.value().type() == typeid(int));
    REQUIRE(std::any_cast<int>(result.value()) == 123);
}

TEST_CASE("meta_property_registry static_set writes value for member pointer property", "[meta_property][set]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    registry.register_object_property<int>("value", &test_object::value);

    test_object obj;
    obj.value = 0;

    auto result = registry.static_set("value", &obj, std::any{42});

    REQUIRE(result);
    REQUIRE(obj.value == 42);
}

TEST_CASE("meta_property_registry static_set returns error on wrong any type", "[meta_property][set][type-mismatch]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    registry.register_object_property<int>("value", &test_object::value);

    test_object obj;
    obj.value = 10;

    auto result = registry.static_set("value", &obj, std::any{std::string{"wrong"}});

    REQUIRE_FALSE(result);
    REQUIRE(obj.value == 10);
}

TEST_CASE("meta_property_registry static_get returns error for unknown property", "[meta_property][get][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    test_object obj;

    auto result = registry.static_get("missing", &obj);

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry static_set returns error for unknown property", "[meta_property][set][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    test_object obj;

    auto result = registry.static_set("missing", &obj, std::any{1});

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry supports custom getter and setter", "[meta_property][custom-getset]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        nullptr,
        [] (test_object* obj) -> int {
            return obj->get_value();
        },
        [] (test_object* obj, const int& v) {
            obj->set_value(v);
        }
    );

    test_object obj;
    obj.value = 77;

    SECTION("custom getter works")
    {
        auto result = registry.static_get("value", &obj);

        REQUIRE(result);
        REQUIRE(result.value().type() == typeid(int));
        REQUIRE(std::any_cast<int>(result.value()) == 77);
    }

    SECTION("custom setter works")
    {
        auto result = registry.static_set("value", &obj, std::any{555});

        REQUIRE(result);
        REQUIRE(obj.value == 555);
    }
}

TEST_CASE("meta_property_registry custom getter/setter override pointer fallback", "[meta_property][custom][override]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        &test_object::value,
        [] (test_object* obj) -> int {
            return obj->value + 1;
        },
        [] (test_object* obj, const int& v) {
            obj->value = v + 10;
        }
    );

    test_object obj;
    obj.value = 5;

    auto get_result = registry.static_get("value", &obj);
    REQUIRE(get_result);
    REQUIRE(std::any_cast<int>(get_result.value()) == 6);

    auto set_result = registry.static_set("value", &obj, std::any{7});
    REQUIRE(set_result);
    REQUIRE(obj.value == 17);
}

TEST_CASE("meta_property_registry supports reset callback", "[meta_property][reset]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        &test_object::value,
        nullptr,
        nullptr,
        nullptr,
        [] (test_object* obj) {
            obj->reset_value();
        }
    );

    test_object obj;
    obj.value = 99;

    REQUIRE(registry.has_reset("value"));

    auto result = registry.static_reset("value", &obj);

    REQUIRE(result);
    REQUIRE(obj.value == -1);
    REQUIRE(obj.reset_count == 1);
}

TEST_CASE("meta_property_registry supports notify callback", "[meta_property][notify]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        &test_object::value,
        nullptr,
        nullptr,
        [] (test_object* obj) {
            obj->notify_value();
            obj->notify_count += 1;
        },
        nullptr
    );

    test_object obj;

    REQUIRE(registry.has_notify("value"));

    auto result = registry.static_notify("value", &obj);

    REQUIRE(result);
    REQUIRE(obj.notify_count == 1);
}

TEST_CASE("meta_property_registry reset returns error when reset function is absent", "[meta_property][reset][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    auto res = registry.register_object_property<int>("value", &test_object::value);

    test_object obj;

    auto result = registry.static_reset("value", &obj);

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry notify returns error when notify function is absent", "[meta_property][notify][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    auto res = registry.register_object_property<int>("value", &test_object::value);

    test_object obj;

    auto result = registry.static_notify("value", &obj);

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry getter is absent when only setter is registered", "[meta_property][getter][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        nullptr,
        nullptr,
        [] (test_object* obj, const int& v) {
            obj->value = v;
        }
    );

    test_object obj;

    REQUIRE_FALSE(registry.has_getter("value"));
    REQUIRE(registry.has_setter("value"));

    auto result = registry.static_get("value", &obj);

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry setter is absent when only getter is registered", "[meta_property][setter][missing]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    auto res = registry.register_object_property<int>(
        "value",
        nullptr,
        [] (test_object* obj) -> int {
            return obj->value;
        },
        nullptr
    );

    test_object obj;

    REQUIRE(registry.has_getter("value"));
    REQUIRE_FALSE(registry.has_setter("value"));

    auto result = registry.static_set("value", &obj, std::any{10});

    REQUIRE_FALSE(result);
}

TEST_CASE("meta_property_registry supports std::string property", "[meta_property][string]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    auto res = registry.register_object_property<std::string>("text", &test_object::text);

    test_object obj;
    obj.text = "before";

    auto set_result = registry.static_set("text", &obj, std::any{std::string{"after"}});
    REQUIRE(set_result);
    REQUIRE(obj.text == "after");

    auto get_result = registry.static_get("text", &obj);
    REQUIRE(get_result);
    REQUIRE(std::any_cast<std::string>(get_result.value()) == "after");
}

TEST_CASE("meta_property_registry throws on duplicate property registration", "[meta_property][register][duplicate]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;
    auto res = registry.register_object_property<int>("value", &test_object::value);

    REQUIRE_THROWS(registry.register_object_property<int>("value", &test_object::value));
}

TEST_CASE("meta_property_registry property_count tracks multiple properties", "[meta_property][count]")
{
    using registry_t = nx::core::detail::meta_property_registry<test_object>;

    registry_t registry;

    REQUIRE(registry.property_count() == 0);

    auto res = registry.register_object_property<int>("value", &test_object::value);
    REQUIRE(registry.property_count() == 1);

    auto res2 = registry.register_object_property<std::string>("text", &test_object::text);
    REQUIRE(registry.property_count() == 2);
}
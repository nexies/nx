//
// test_nx_property — NX_PROPERTY macro v2 integration tests.
//
// Covers the various keyword combinations that the macro supports:
//   TYPE, MEMBER, READ, WRITE, RESET, NOTIFY, DEFAULT, CONST
//

#include <catch2/catch_test_macros.hpp>
#include <nx/core2/object/object.hpp>
#include <nx/core2/detail/property_defs2.hpp>

#include <any>
#include <string>

namespace {

// ─── Helper: tracks received signal emissions ─────────────────────────────

struct int_sink : nx::core::object {
    NX_OBJECT(int_sink)
    int   last  { -1 };
    int   count { 0  };
    void  on_int(int v) { last = v; ++count; }
};

struct str_sink : nx::core::object {
    NX_OBJECT(str_sink)
    std::string last;
    int         count { 0 };
    void        on_str(std::string v) { last = std::move(v); ++count; }
};

// ─── Test classes — one per configuration ────────────────────────────────

// 1) TYPE + READ + WRITE — auto getter/setter, meta getter/setter
struct obj_rw : nx::core::object {
    NX_OBJECT(obj_rw)
    NX_PROPERTY(score, TYPE int, READ, WRITE)
};

// 2) TYPE + READ + WRITE + bare NOTIFY — auto signal, setter emits it
struct obj_notify : nx::core::object {
    NX_OBJECT(obj_notify)
    NX_PROPERTY(level, TYPE int, READ, WRITE, NOTIFY)
};

// 3) TYPE + READ + WRITE + named NOTIFY — use existing signal
struct obj_named_notify : nx::core::object {
    NX_OBJECT(obj_named_notify)
    NX_SIGNAL(on_name_signal, std::string)
    NX_PROPERTY(label, TYPE std::string, READ, WRITE, NOTIFY on_name_signal)
};

// 4) TYPE + READ + WRITE + RESET + DEFAULT — reset to explicit default
struct obj_reset : nx::core::object {
    NX_OBJECT(obj_reset)
    NX_PROPERTY(hp, TYPE int, READ, WRITE, RESET, DEFAULT 100)
};

// 5) TYPE + RESET (no READ/WRITE) — reset via meta, primitive lambdas for get/set
struct obj_reset_only : nx::core::object {
    NX_OBJECT(obj_reset_only)
    NX_PROPERTY(mana, TYPE int, RESET, DEFAULT 50)
};

// 6) TYPE only — no generated methods, primitive lambda getter/setter in meta
struct obj_type_only : nx::core::object {
    NX_OBJECT(obj_type_only)
    NX_PROPERTY(data, TYPE int)
};

// 7) TYPE + READ only — generated getter, primitive setter registered in meta
struct obj_readonly : nx::core::object {
    NX_OBJECT(obj_readonly)
    NX_PROPERTY(token, TYPE std::string, READ)
};

// 8) MEMBER — expose existing member via meta primitive lambdas
struct obj_member : nx::core::object {
    NX_OBJECT(obj_member)
    int count { 7 };
    NX_PROPERTY(counter, MEMBER count)
};

// 9) MEMBER + READ + WRITE — existing member, auto accessor methods
struct obj_member_rw : nx::core::object {
    NX_OBJECT(obj_member_rw)
    std::string m_title { "default" };
    NX_PROPERTY(title, MEMBER m_title, READ, WRITE)
};

// 10) MEMBER + READ + WRITE + NOTIFY (named) — external member, signal on set.
// Bare NOTIFY with MEMBER is not supported: NX_SIGNAL would need the inferred
// type as its parameter, which requires the class to be complete at the point
// of the signal declaration.  Use a named NOTIFY (pre-declare the signal).
struct obj_member_notify : nx::core::object {
    NX_OBJECT(obj_member_notify)
    int m_pts { 0 };
    NX_SIGNAL(pts_changed, int)
    NX_PROPERTY(pts, MEMBER m_pts, READ, WRITE, NOTIFY pts_changed)
};

// 11) CONST + TYPE + READ — no setter registered
struct obj_const : nx::core::object {
    NX_OBJECT(obj_const)
    NX_PROPERTY(uid, TYPE int, READ, CONST)
};

// 12) Computed: bare READ with explicit method name — getter only, type inferred
struct obj_computed : nx::core::object {
    NX_OBJECT(obj_computed)
    int x { 3 };
    int doubled() const { return x * 2; }
    NX_PROPERTY(double_x, READ doubled)
};

} // namespace

// ─── Tests: TYPE + READ + WRITE ───────────────────────────────────────────

TEST_CASE("NX_PROPERTY TYPE+READ+WRITE: generated getter and setter work", "[nx_property][rw]")
{
    obj_rw obj;
    REQUIRE(obj.get_score() == 0);
    obj.set_score(42);
    REQUIRE(obj.get_score() == 42);
}

TEST_CASE("NX_PROPERTY TYPE+READ+WRITE: meta registry has getter+setter only", "[nx_property][rw][meta]")
{
    auto& reg = obj_rw::static_meta_object().property_registry();
    REQUIRE(reg.has_property("score"));
    REQUIRE(reg.has_getter("score"));
    REQUIRE(reg.has_setter("score"));
    REQUIRE_FALSE(reg.has_reset("score"));
    REQUIRE_FALSE(reg.has_notify("score"));
}

TEST_CASE("NX_PROPERTY TYPE+READ+WRITE: static_get and static_set via meta", "[nx_property][rw][meta]")
{
    auto& reg = obj_rw::static_meta_object().property_registry();
    obj_rw obj;
    obj.set_score(99);

    auto got = reg.static_get("score", &obj);
    REQUIRE(got);
    REQUIRE(std::any_cast<int>(got.value()) == 99);

    auto set_res = reg.static_set("score", &obj, std::any{7});
    REQUIRE(set_res);
    REQUIRE(obj.get_score() == 7);
}

TEST_CASE("NX_PROPERTY TYPE+READ+WRITE: static_set rejects wrong type", "[nx_property][rw][meta]")
{
    auto& reg = obj_rw::static_meta_object().property_registry();
    obj_rw obj;
    auto res = reg.static_set("score", &obj, std::any{std::string{"oops"}});
    REQUIRE_FALSE(res);
}

// ─── Tests: TYPE + READ + WRITE + bare NOTIFY ─────────────────────────────

TEST_CASE("NX_PROPERTY bare NOTIFY: setter emits auto-generated signal", "[nx_property][notify]")
{
    obj_notify obj;
    int_sink   sink;
    nx::core::connect(&obj, &obj_notify::level_changed, &sink, &int_sink::on_int);

    obj.set_level(5);
    REQUIRE(sink.last  == 5);
    REQUIRE(sink.count == 1);
}

TEST_CASE("NX_PROPERTY bare NOTIFY: meta has notify registered", "[nx_property][notify][meta]")
{
    auto& reg = obj_notify::static_meta_object().property_registry();
    REQUIRE(reg.has_notify("level"));
}

TEST_CASE("NX_PROPERTY bare NOTIFY: static_notify emits signal with current value", "[nx_property][notify][meta]")
{
    auto& reg = obj_notify::static_meta_object().property_registry();
    obj_notify obj;
    int_sink   sink;
    nx::core::connect(&obj, &obj_notify::level_changed, &sink, &int_sink::on_int);

    obj.set_level(9);
    REQUIRE(sink.count == 1);

    auto res = reg.static_notify("level", &obj);
    REQUIRE(res);
    REQUIRE(sink.count == 2);
    REQUIRE(sink.last  == 9);
}

// ─── Tests: TYPE + READ + WRITE + named NOTIFY ───────────────────────────

TEST_CASE("NX_PROPERTY named NOTIFY: setter emits existing signal", "[nx_property][notify][named]")
{
    obj_named_notify obj;
    str_sink         sink;
    nx::core::connect(&obj, &obj_named_notify::on_name_signal, &sink, &str_sink::on_str);

    obj.set_label("hello");
    REQUIRE(sink.last  == "hello");
    REQUIRE(sink.count == 1);
}

// ─── Tests: TYPE + READ + WRITE + RESET + DEFAULT ────────────────────────

TEST_CASE("NX_PROPERTY RESET+DEFAULT: field initialized to default", "[nx_property][reset]")
{
    obj_reset obj;
    REQUIRE(obj.get_hp() == 100);
}

TEST_CASE("NX_PROPERTY RESET+DEFAULT: set then reset restores default", "[nx_property][reset]")
{
    obj_reset obj;
    obj.set_hp(1);
    REQUIRE(obj.get_hp() == 1);
    obj.reset_hp();
    REQUIRE(obj.get_hp() == 100);
}

TEST_CASE("NX_PROPERTY RESET+DEFAULT: meta has reset registered", "[nx_property][reset][meta]")
{
    auto& reg = obj_reset::static_meta_object().property_registry();
    REQUIRE(reg.has_reset("hp"));
}

TEST_CASE("NX_PROPERTY RESET+DEFAULT: static_reset via meta restores default", "[nx_property][reset][meta]")
{
    auto& reg = obj_reset::static_meta_object().property_registry();
    obj_reset obj;
    obj.set_hp(1);
    auto res = reg.static_reset("hp", &obj);
    REQUIRE(res);
    REQUIRE(obj.get_hp() == 100);
}

// ─── Tests: TYPE + RESET only (no READ/WRITE) ────────────────────────────

TEST_CASE("NX_PROPERTY TYPE+RESET: primitive lambda getter/setter, meta has reset", "[nx_property][reset][lambda]")
{
    auto& reg = obj_reset_only::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("mana"));
    REQUIRE(reg.has_setter("mana"));
    REQUIRE(reg.has_reset("mana"));

    obj_reset_only obj;

    auto set_res = reg.static_set("mana", &obj, std::any{10});
    REQUIRE(set_res);

    auto reset_res = reg.static_reset("mana", &obj);
    REQUIRE(reset_res);

    auto got = reg.static_get("mana", &obj);
    REQUIRE(got);
    REQUIRE(std::any_cast<int>(got.value()) == 50);
}

// ─── Tests: TYPE only ────────────────────────────────────────────────────

TEST_CASE("NX_PROPERTY TYPE only: meta has primitive lambda getter and setter", "[nx_property][type_only][meta]")
{
    auto& reg = obj_type_only::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("data"));
    REQUIRE(reg.has_setter("data"));
    REQUIRE_FALSE(reg.has_reset("data"));
    REQUIRE_FALSE(reg.has_notify("data"));
}

TEST_CASE("NX_PROPERTY TYPE only: static_get and static_set work", "[nx_property][type_only][meta]")
{
    auto& reg = obj_type_only::static_meta_object().property_registry();
    obj_type_only obj;

    auto set_res = reg.static_set("data", &obj, std::any{55});
    REQUIRE(set_res);

    auto got = reg.static_get("data", &obj);
    REQUIRE(got);
    REQUIRE(std::any_cast<int>(got.value()) == 55);
}

// ─── Tests: TYPE + READ only ──────────────────────────────────────────────

TEST_CASE("NX_PROPERTY TYPE+READ: generated getter works", "[nx_property][readonly]")
{
    obj_readonly obj;
    obj.m_property_token = "abc";
    REQUIRE(obj.get_token() == "abc");
}

TEST_CASE("NX_PROPERTY TYPE+READ: no setter in meta", "[nx_property][readonly_const][meta]")
{
    auto& reg = obj_const::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("uid"));
    REQUIRE_FALSE(reg.has_setter("uid"));

    obj_const obj;
    auto res = reg.static_set("uid", &obj, std::any{std::string{"x"}});
    REQUIRE_FALSE(res);
}

// ─── Tests: MEMBER ────────────────────────────────────────────────────────

TEST_CASE("NX_PROPERTY MEMBER: meta has getter and setter via primitive lambdas", "[nx_property][member][meta]")
{
    auto& reg = obj_member::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("counter"));
    REQUIRE(reg.has_setter("counter"));
}

TEST_CASE("NX_PROPERTY MEMBER: static_get returns existing member value", "[nx_property][member][meta]")
{
    auto& reg = obj_member::static_meta_object().property_registry();
    obj_member obj;
    REQUIRE(obj.count == 7);

    auto got = reg.static_get("counter", &obj);
    REQUIRE(got);
    REQUIRE(std::any_cast<int>(got.value()) == 7);
}

TEST_CASE("NX_PROPERTY MEMBER: static_set writes to existing member", "[nx_property][member][meta]")
{
    auto& reg = obj_member::static_meta_object().property_registry();
    obj_member obj;

    auto res = reg.static_set("counter", &obj, std::any{42});
    REQUIRE(res);
    REQUIRE(obj.count == 42);
}

// ─── Tests: MEMBER + READ + WRITE ────────────────────────────────────────

TEST_CASE("NX_PROPERTY MEMBER+READ+WRITE: generated getter reads existing member", "[nx_property][member][rw]")
{
    obj_member_rw obj;
    REQUIRE(obj.get_title() == "default");
}

TEST_CASE("NX_PROPERTY MEMBER+READ+WRITE: generated setter writes existing member", "[nx_property][member][rw]")
{
    obj_member_rw obj;
    obj.set_title("updated");
    REQUIRE(obj.get_title()  == "updated");
    REQUIRE(obj.m_title      == "updated");
}

TEST_CASE("NX_PROPERTY MEMBER+READ+WRITE: meta getter/setter registered", "[nx_property][member][rw][meta]")
{
    auto& reg = obj_member_rw::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("title"));
    REQUIRE(reg.has_setter("title"));
}

// ─── Tests: MEMBER + READ + WRITE + NOTIFY ───────────────────────────────

TEST_CASE("NX_PROPERTY MEMBER+READ+WRITE+NOTIFY: setter emits auto signal", "[nx_property][member][notify]")
{
    obj_member_notify obj;
    int_sink          sink;
    nx::core::connect(&obj, &obj_member_notify::pts_changed, &sink, &int_sink::on_int);

    obj.set_pts(77);
    REQUIRE(sink.last  == 77);
    REQUIRE(sink.count == 1);
    REQUIRE(obj.m_pts  == 77);
}

// ─── Tests: CONST + TYPE + READ ──────────────────────────────────────────

TEST_CASE("NX_PROPERTY CONST+READ: meta has getter but no setter", "[nx_property][const][meta]")
{
    auto& reg = obj_const::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("uid"));
    REQUIRE_FALSE(reg.has_setter("uid"));
    REQUIRE_FALSE(reg.has_reset("uid"));
}

TEST_CASE("NX_PROPERTY CONST+READ: static_set is rejected", "[nx_property][const][meta]")
{
    auto& reg = obj_const::static_meta_object().property_registry();
    obj_const obj;
    auto res = reg.static_set("uid", &obj, std::any{1});
    REQUIRE_FALSE(res);
}

// ─── Tests: computed READ with explicit method ────────────────────────────

TEST_CASE("NX_PROPERTY computed READ: meta getter uses provided method", "[nx_property][computed]")
{
    auto& reg = obj_computed::static_meta_object().property_registry();
    REQUIRE(reg.has_getter("double_x"));
    REQUIRE_FALSE(reg.has_setter("double_x"));
}

TEST_CASE("NX_PROPERTY computed READ: static_get returns method result", "[nx_property][computed][meta]")
{
    auto& reg = obj_computed::static_meta_object().property_registry();
    obj_computed obj;
    obj.x = 5;

    auto got = reg.static_get("double_x", &obj);
    REQUIRE(got);
    REQUIRE(std::any_cast<int>(got.value()) == 10);
}

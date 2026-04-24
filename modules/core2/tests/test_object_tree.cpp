//
// test_object_tree — contracts for the core2 parent/child object tree.
//

#include <nx/core2/object/object.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

// A node whose destructor records that it was destroyed.
struct tracked_node : nx::core::object {
    NX_OBJECT(tracked_node)

    bool * destroyed_flag { nullptr };

    explicit tracked_node(nx::core::object * parent = nullptr)
        : nx::core::object(parent)
    {}

    explicit tracked_node(bool & flag, nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , destroyed_flag(&flag)
    {}

    ~tracked_node() override
    {
        if (destroyed_flag)
            *destroyed_flag = true;
    }
};

// Observer that records the 'destroyed' signal
struct observer : nx::core::object {
    NX_OBJECT(observer)

    int destroyed_count { 0 };

    void on_destroyed() { ++destroyed_count; }

    using nx::core::object::object;
};

} // namespace

// ── Parent / child linkage ────────────────────────────────────────────────────

TEST_CASE("object tree: child reports correct parent", "[object_tree]")
{
    tracked_node parent;
    tracked_node child(&parent);

    REQUIRE(child.parent() == &parent);
}

TEST_CASE("object tree: parent has child in children() list", "[object_tree]")
{
    tracked_node parent;
    tracked_node child(&parent);

    const auto kids = parent.children();
    REQUIRE(kids.size() == 1);
    REQUIRE(kids[0] == &child);
}

TEST_CASE("object tree: multiple children are all listed", "[object_tree]")
{
    tracked_node parent;
    tracked_node c1(&parent), c2(&parent), c3(&parent);

    REQUIRE(parent.children().size() == 3);
}

TEST_CASE("object tree: root object has null parent", "[object_tree]")
{
    tracked_node root;
    REQUIRE(root.parent() == nullptr);
}

TEST_CASE("object tree: root object has empty children list", "[object_tree]")
{
    tracked_node root;
    REQUIRE(root.children().empty());
}

// ── set_parent ────────────────────────────────────────────────────────────────

TEST_CASE("object tree: set_parent reparents to new parent", "[object_tree]")
{
    tracked_node p1, p2;
    tracked_node child(&p1);

    child.set_parent(&p2);

    REQUIRE(child.parent() == &p2);
    REQUIRE(p1.children().empty());
    REQUIRE(p2.children().size() == 1);
}

TEST_CASE("object tree: set_parent with null detaches from parent", "[object_tree]")
{
    tracked_node parent;
    tracked_node child(&parent);

    child.set_parent(nullptr);

    REQUIRE(child.parent() == nullptr);
    REQUIRE(parent.children().empty());
}

TEST_CASE("object tree: set_parent with same parent is a no-op", "[object_tree]")
{
    tracked_node parent;
    tracked_node child(&parent);

    child.set_parent(&parent); // same parent — must not duplicate

    REQUIRE(parent.children().size() == 1);
}

// ── Cascade destruction ───────────────────────────────────────────────────────

TEST_CASE("object tree: destroying parent destroys heap-allocated child", "[object_tree]")
{
    bool child_destroyed = false;

    {
        tracked_node parent;
        new tracked_node(child_destroyed, &parent); // owned by parent
        REQUIRE_FALSE(child_destroyed);
    } // parent destroyed here → must delete the child

    REQUIRE(child_destroyed);
}

TEST_CASE("object tree: grandchild is destroyed when grandparent is destroyed", "[object_tree]")
{
    bool grandchild_destroyed = false;

    {
        tracked_node grandparent;
        tracked_node parent(&grandparent);
        new tracked_node(grandchild_destroyed, &parent);

        REQUIRE_FALSE(grandchild_destroyed);
    }

    REQUIRE(grandchild_destroyed);
}

TEST_CASE("object tree: destroyed signal fires on destruction", "[object_tree]")
{
    observer obs;
    {
        tracked_node n;
        nx::core::connect_direct(&n, &tracked_node::destroyed,
                                 &obs, &observer::on_destroyed);
    } // n destroyed here

    REQUIRE(obs.destroyed_count == 1);
}

TEST_CASE("object tree: destroyed signal fires for each child on cascade", "[object_tree]")
{
    observer obs;
    {
        tracked_node parent;
        // Two heap children — both destroyed when parent is destroyed
        auto * c1 = new tracked_node(&parent);
        auto * c2 = new tracked_node(&parent);
        nx::core::connect_direct(c1, &tracked_node::destroyed,
                                 &obs, &observer::on_destroyed);
        nx::core::connect_direct(c2, &tracked_node::destroyed,
                                 &obs, &observer::on_destroyed);
    }

    REQUIRE(obs.destroyed_count == 2);
}

// ── Detached child is not destroyed with former parent ────────────────────────

TEST_CASE("object tree: detached child survives parent destruction", "[object_tree]")
{
    bool child_destroyed = false;

    tracked_node child(child_destroyed); // no parent

    {
        tracked_node parent;
        child.set_parent(&parent);
        child.set_parent(nullptr); // detach before parent goes out of scope
    }

    REQUIRE_FALSE(child_destroyed);
}

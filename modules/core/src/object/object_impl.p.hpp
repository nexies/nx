//
// Created by nexie on 17.03.2026.
//

#ifndef NX_OBJECTIMPL_HPP
#define NX_OBJECTIMPL_HPP

#include <nx/core/Object.hpp>

using namespace nx;

class Object::Impl {
    friend class Object;
    // struct ChildrenSet {
    //     std::unordered_set<Object *> children;
    //
    //     void add(Object *child) {
    //         children.insert(child);
    //     }
    //     void remove(Object *child) {
    //         children.erase(child);
    //     }
    // };

    Object * obj_;
    Object * parent_;
    std::vector<Object*> children_;
    Thread * local_thread_;
    std::unique_ptr<ConnectionInfo> connection_info_;

public:
    explicit
    Impl(Object * obj) noexcept;

    ~Impl();

    void
    addChildObject (Object * child);

    void
    removeChildObject (Object * child);

    [[nodiscard]] bool
    isTopLevelObject () const;

    void
    detachFromParentObject ();

    void
    attachToParentObject (Object * parent);

    Object *
    getParentObject () const;

    [[nodiscard]] ThreadId
    getThreadObjectId () const;

    [[nodiscard]] Thread*
    getThreadObject() const;

    void
    detachFromThreadObject();

    void
    attachToThreadObject(Thread* thread) noexcept;

};

#endif //NX_OBJECTIMPL_HPP

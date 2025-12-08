//
// Created by nexie on 25.11.2025.
//

#ifndef PROPERTY_DEFS_HPP
#define PROPERTY_DEFS_HPP

#include "nx/core/macro/repeat.hpp"
#include "nx/core/macro/arg_count.hpp"
#include "nx/core/macro/choose.hpp"
#include "nx/core/macro/logic.hpp"

// clang-tidy: -cppcoreguidelines-avoid-non-const-global-variables

// clang-tidy: disable cppcoreguidelines-avoid-non-const-global-variables
// NOLINTBEGIN(readability-identifier-naming)
#define __NX_PROPERTY_SELECT_TRAIT(...) __NX_PROPERTY_SELECT_TRAIT_(__VA_ARGS__)
#define __NX_PROPERTY_SELECT_TRAIT_(name, tk_name, tk_val) __NX_PROPERTY_SELECT_TRAIT_##name##_FROM_##tk_name(tk_val)

#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_TYPE(t) t
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_TYPE_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_NAME(n) n
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_NAME_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_READ(r) r
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_READ_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_WRITE(w) w
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_WRITE_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_NOTIFY(n) n
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_NOTIFY_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_DEFAULT(d) d
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_DEFAULT_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_RESET(r) r
#define __NX_PROPERTY_SELECT_TRAIT_RESET_FROM_MEMBER(m)

#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_TYPE(t)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_NAME(n)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_READ(r)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_WRITE(w)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_NOTIFY(n)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_DEFAULT(d)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_RESET(r)
#define __NX_PROPERTY_SELECT_TRAIT_MEMBER_FROM_MEMBER(m) m

#define __NX_PROPERTY_EXPAND_TK_TYPE(t) TYPE, t
#define __NX_PROPERTY_EXPAND_TK_NAME(n) NAME, n
#define __NX_PROPERTY_EXPAND_TK_READ(r) READ, r
#define __NX_PROPERTY_EXPAND_TK_WRITE(w) WRITE, w
#define __NX_PROPERTY_EXPAND_TK_NOTIFY(n) NOTIFY, n
#define __NX_PROPERTY_EXPAND_TK_DEFAULT(d) DEFAULT, d
#define __NX_PROPERTY_EXPAND_TK_RESET(r) RESET, r
#define __NX_PROPERTY_EXPAND_TK_MEMBER(m) MEMBER, m

#define __NX_PROPERTY_TRAIT_TYPE_(t) _TK_TYPE(t)
#define __NX_PROPERTY_TRAIT_NAME_(n) _TK_NAME(n)
#define __NX_PROPERTY_TRAIT_READ_(r) _TK_READ(r)
#define __NX_PROPERTY_TRAIT_WRITE_(w) _TK_WRITE(w)
#define __NX_PROPERTY_TRAIT_NOTIFY_(n) _TK_NOTIFY(n)
#define __NX_PROPERTY_TRAIT_DEFAULT_(d) _TK_DEFAULT(d)
#define __NX_PROPERTY_TRAIT_RESET_(r) _TK_RESET(r)
#define __NX_PROPERTY_TRAIT_MEMBER_(m) _TK_MEMBER(m)

#define __NX_PROPERTY_TRAIT_TYPE __NX_PROPERTY_TRAIT_TYPE_(
#define __NX_PROPERTY_TRAIT_NAME __NX_PROPERTY_TRAIT_NAME_(
#define __NX_PROPERTY_TRAIT_READ __NX_PROPERTY_TRAIT_READ_(
#define __NX_PROPERTY_TRAIT_WRITE __NX_PROPERTY_TRAIT_WRITE_(
#define __NX_PROPERTY_TRAIT_NOTIFY __NX_PROPERTY_TRAIT_NOTIFY_(
#define __NX_PROPERTY_TRAIT_DEFAULT __NX_PROPERTY_TRAIT_DEFAULT_(
#define __NX_PROPERTY_TRAIT_RESET __NX_PROPERTY_TRAIT_RESET_(
#define __NX_PROPERTY_TRAIT_MEMBER __NX_PROPERTY_TRAIT_MEMBER_(
#define __NX_PROPERTY_IT_CONVERT_TRAIT_TOKEN(n, ...) NX_CONCAT(__NX_PROPERTY_TRAIT_, NX_CHOOSE(n, ,__VA_ARGS__)) )

#define __NX_PROPERTY_IT_FIND_TRAIT(n, trait, ...) \
    __NX_PROPERTY_SELECT_TRAIT(trait, NX_EXPAND(NX_CONCAT(__NX_PROPERTY_EXPAND, NX_CHOOSE(n, , __VA_ARGS__))))

#define __NX_PROPERTY_FIND_TRAIT_TOKEN_(trait, ...) \
    NX_REPEAT_NO_COMMA(NX_ARGS_COUNT(__VA_ARGS__), __NX_PROPERTY_IT_FIND_TRAIT, trait, __VA_ARGS__)

#define __NX_PROPERTY_FIND_TRAIT_TOKEN(...) \
    __NX_PROPERTY_FIND_TRAIT_TOKEN_(__VA_ARGS__)

#define __NX_PROPERTY_HAS_TRAIT_TOKEN_(trait, ...) \
    NX_APPLY(NX_HAS_ARGS, NX_EXPAND(__NX_PROPERTY_FIND_TRAIT_TOKEN(trait, __VA_ARGS__)))

#define __NX_PROPERTY_HAS_TRAIT_TOKEN(...) \
    __NX_PROPERTY_HAS_TRAIT_TOKEN_(__VA_ARGS__)

#define __NX_PROPERTY_CONVERT_TO_TRAIT_TOKENS(...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_PROPERTY_IT_CONVERT_TRAIT_TOKEN, __VA_ARGS__)

#define __NX_PROPERTY_FIND_TRAIT(trait, ...) \
    __NX_PROPERTY_FIND_TRAIT_TOKEN(trait, __NX_PROPERTY_CONVERT_TO_TRAIT_TOKENS(__VA_ARGS__))

#define __NX_PROPERTY_HAS_TRAIT(trait, ...) \
    __NX_PROPERTY_HAS_TRAIT_TOKEN(trait, __NX_PROPERTY_CONVERT_TO_TRAIT_TOKENS(__VA_ARGS__))

#define __NX_PROPERTY_HAS_TYPE_AND_NAME_OR_TYPE_AND_MEMBER(...) \
    NX_EXPAND(\
    __NX_XOR \
    (__NX_AND(__NX_PROPERTY_HAS_TRAIT(TYPE, __VA_ARGS__), __NX_PROPERTY_HAS_TRAIT(NAME, __VA_ARGS__)), \
             __NX_AND(__NX_PROPERTY_HAS_TRAIT(TYPE, __VA_ARGS__), __NX_PROPERTY_HAS_TRAIT(MEMBER, __VA_ARGS__)))

#define __NX_PROPERTY_FIND_NAME_0(...) \
    __NX_PROPERTY_FIND_TRAIT(MEMBER, __VA_ARGS__)

#define __NX_PROPERTY_FIND_NAME_1(...) \
    __NX_PROPERTY_FIND_TRAIT(NAME, __VA_ARGS__)

#define __NX_PROPERTY_FIND_NAME(...) \
    NX_CONCAT(__NX_PROPERTY_FIND_NAME_, __NX_PROPERTY_HAS_TRAIT(NAME, __VA_ARGS__))(__VA_ARGS__)

/// CHECK_1 - check if a property has a type and a name
#define __NX_PROPERTY_CHECK_1(...) \
    NX_CONCAT(__NX_PROPERTY_CHECK_1_, __NX_PROPERTY_HAS_TYPE_AND_NAME_OR_TYPE_AND_MEMBER(__VA_ARGS__))(__VA_ARGS__)

#define __NX_PROPERTY_CHECK_1_0(...) \
    static_assert(false, "NX_PROPERTY must have TYPE and NAME traits or TYPE and MEMBER traits")

#define __NX_PROPERTY_CHECK_1_1(...) \
    __NX_PROPERTY_CREATE(__VA_ARGS__)

/// CHECK_2 - check that property trait TYPE does not appear twice
#define __NX_PROPERTY_CHECK_2(...) \

#define __NX_PROPERTY_CHECK(...) \
    __NX_PROPERTY_CHECK_1(__VA_ARGS__)

#define __NX_PROPERTY(...) \
    __NX_PROPERTY_CHECK(__VA_ARGS__)

#define __NX_PROPERTY_CREATE(...) \
    __NX_PROPERTY_CREATE_( \
        __NX_PROPERTY_FIND_TRAIT(TYPE, __VA_ARGS__), \
        __NX_PROPERTY_FIND_NAME(__VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(MEMBER, __VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(READ, __VA_ARGS__), __NX_PROPERTY_FIND_TRAIT(READ, __VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(WRITE, __VA_ARGS__), __NX_PROPERTY_FIND_TRAIT(WRITE, __VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(NOTIFY, __VA_ARGS__), __NX_PROPERTY_FIND_TRAIT(NOTIFY, __VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(DEFAULT, __VA_ARGS__), __NX_PROPERTY_FIND_TRAIT(DEFAULT, __VA_ARGS__), \
        __NX_PROPERTY_HAS_TRAIT(RESET, __VA_ARGS__), __NX_PROPERTY_FIND_TRAIT(RESET, __VA_ARGS__))

#define __NX_PROPERTY_MEMBER_PREFIX m_
#define __NX_PROPERTY_NAME_TO_MEMBER(name) NX_CONCAT(__NX_PROPERTY_MEMBER_PREFIX, name)

#define __NX_PROPERTY_CREATE_DECL_0_0(type, name, ...) \
    type __NX_PROPERTY_NAME_TO_MEMBER(name);

#define __NX_PROPERTY_CREATE_DECL_0_1(type, name, default_value) \
    type __NX_PROPERTY_NAME_TO_MEMBER(name) = default_value;

#define __NX_PROPERTY_CREATE_DECL_0(type, name, default_value, has_default) \
    NX_CONCAT(__NX_PROPERTY_CREATE_DECL_0_, has_default)(type, name, default_value)

#define __NX_PROPERTY_CREATE_DECL_1(...) NX_CONSUME(__VA_ARGS__)

#define __NX_PROPERTY_CREATE_DECL(type, name, has_default, default_value, is_member) \
    NX_CONCAT(__NX_PROPERTY_CREATE_DECL_, is_member)(type, name, default_value, has_default)

#define __NX_PROPERTY_CREATE_GETTER_0(...) NX_CONSUME(__VA_ARGS__)
#define __NX_PROPERTY_CREATE_GETTER_1(type, name, getter_name) \
    type getter_name() const { return __NX_PROPERTY_NAME_TO_MEMBER(name); }

#define __NX_PROPERTY_CREATE_GETTER(type, name, has_getter, getter_name) \
   NX_CONCAT(__NX_PROPERTY_CREATE_GETTER_, has_getter)(type, name, getter_name)

#define __NX_PROPERTY_CREATE_NOTIFY_0(...) NX_CONSUME(__VA_ARGS__)
#define __NX_PROPERTY_CREATE_NOTIFY_1(type, name, notify_name) \
    NX_SIGNAL(notify_name, type)

#define __NX_PROPERTY_CREATE_NOTIFY(type, name, has_notify, notify_name) \
    NX_CONCAT(__NX_PROPERTY_CREATE_NOTIFY_, has_notify)(type, name, notify_name)

#define __NX_PROPERTY_CREATE_SETTER_0(...) NX_CONSUME(__VA_ARGS__)
#define __NX_PROPERTY_CREATE_SETTER_1_0(type, name, setter_name, ...) \
    void setter_name (const type & new_##name) \
    { \
        __NX_PROPERTY_NAME_TO_MEMBER(name) = new_##name;\
    }
#define __NX_PROPERTY_CREATE_SETTER_1_1(type, name, setter_name, notify_name) \
    void setter_name (const type & new_##name) \
    { \
        if(__NX_PROPERTY_NAME_TO_MEMBER(name) == new_##name) \
            return; \
        __NX_PROPERTY_NAME_TO_MEMBER(name) = new_##name;\
        NX_EMIT(notify_name, new_##name); \
    }
#define __NX_PROPERTY_CREATE_SETTER_1(type, name, setter_name, has_notify, notify_name) \
    NX_CONCAT(__NX_PROPERTY_CREATE_SETTER_1_, has_notify)(type, name, setter_name, notify_name)

#define __NX_PROPERTY_CREATE_SETTER(type, name, has_setter, setter_name, has_notify, notify_name) \
    NX_CONCAT(__NX_PROPERTY_CREATE_SETTER_, has_setter)(type, name, setter_name, has_notify, notify_name)

#define __NX_PROPERTY_CREATE_RESET_0(...) NX_CONSUME(__VA_ARGS__)
#define __NX_PROPERTY_CREATE_RESET_CREATE_NOTIFY_0(...) NX_CONSUME(__VA_ARGS__)
#define __NX_PROPERTY_CREATE_RESET_CREATE_NOTIFY_1(cur_val, new_val, notify_name) \
    NX_EMIT(notify_name, new_val);
#define __NX_PROPERTY_CREATE_RESET_CREATE_NOTIFY(cur_val, new_val, has_notify, notify_name) \
    NX_CONCAT(__NX_PROPERTY_CREATE_RESET_CREATE_NOTIFY_, has_notify)(cur_val, new_val, notify_name)
#define __NX_PROPERTY_CREATE_RESET_RESET_VALUE_0(type, ...) type()
#define __NX_PROPERTY_CREATE_RESET_RESET_VALUE_1(type, default_value) default_value
#define __NX_PROPERTY_CREATE_RESET_RESET_VALUE(type, has_default, default_value) \
    NX_CONCAT(__NX_PROPERTY_CREATE_RESET_RESET_VALUE_, has_default)(type, default_value)

#define __NX_PROPERTY_CREATE_RESET_1(type, name, reset_name, has_default, default_value, has_notify, notify_name) \
    void reset_name () { \
        if(__NX_PROPERTY_NAME_TO_MEMBER(name) == __NX_PROPERTY_CREATE_RESET_RESET_VALUE(type, has_default, default_value)) \
            return; \
        __NX_PROPERTY_NAME_TO_MEMBER(name) = __NX_PROPERTY_CREATE_RESET_RESET_VALUE(type, has_default, default_value); \
        __NX_PROPERTY_CREATE_RESET_CREATE_NOTIFY( \
            __NX_PROPERTY_NAME_TO_MEMBER(name), \
            __NX_PROPERTY_CREATE_RESET_RESET_VALUE(type, has_default, default_value), \
            has_notify, \
            notify_name ) \
    }

#define __NX_PROPERTY_CREATE_RESET(type, name, has_reset, reset_name, has_default, default_value, has_notify, notify_name) \
    NX_CONCAT(__NX_PROPERTY_CREATE_RESET_, has_reset)(type, name, reset_name, has_default, default_value, has_notify, notify_name)


#define __NX_PROPERTY_CREATE_(type, name, is_member, \
                            has_getter, getter_name, \
                            has_setter, setter_name, \
                            has_notify, notify_name, \
                            has_default, default_value, \
                            has_reset, reset_name) \
    __NX_PROPERTY_CREATE_DECL(type, name, has_default, default_value, is_member) \
    __NX_PROPERTY_CREATE_GETTER(type, name, has_getter, getter_name) \
    __NX_PROPERTY_CREATE_NOTIFY(type, name, has_notify, notify_name) \
    __NX_PROPERTY_CREATE_SETTER(type, name, has_setter, setter_name, has_notify, notify_name) \
    __NX_PROPERTY_CREATE_RESET(type, name, has_reset, reset_name, has_default, default_value, has_notify, notify_name)


#define NX_PROPERTY(...) \
    __NX_PROPERTY(__VA_ARGS__)

// NOLINTEND(readability-identifier-naming)
#endif //PROPERTY_DEFS_HPP

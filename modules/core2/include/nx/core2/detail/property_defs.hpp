// //
// // property_defs — NX_PROPERTY macro for nx::core2.
// //
// // Usage:
// //   NX_PROPERTY(TYPE int,         NAME count,
// //               READ count,       WRITE set_count,
// //               NOTIFY count_changed)
// //
// //   NX_PROPERTY(TYPE std::string, NAME label,
// //               READ label,       WRITE set_label,
// //               NOTIFY label_changed, DEFAULT "")
// //
// // Generated code (example for `count` above):
// //   private:
// //     int count_;          // trailing-underscore member
// //   public:
// //     int count() const { return count_; }
// //     NX_SIGNAL(count_changed, int)
// //     void set_count(const int & new_count) {
// //         if (count_ == new_count) return;
// //         count_ = new_count;
// //         NX_EMIT(count_changed, new_count);
// //     }
// //
// // Supported traits:
// //   TYPE     — property type (required)
// //   NAME     — property name → getter, signal and member suffix (required unless MEMBER given)
// //   MEMBER   — use existing member variable instead of generating one
// //   READ     — getter method name
// //   WRITE    — setter method name
// //   NOTIFY   — signal emitted on value change
// //   DEFAULT  — default value for the generated member
// //   RESET    — reset method name (resets to DEFAULT or T{})
// //
//
// #pragma once
//
// #include <nx/macro/repeating/repeat.hpp>
// #include <nx/macro/args/count.hpp>
// #include <nx/macro/util/choose.hpp>
// #include <nx/macro/logic.hpp>
// #include <nx/core2/detail/signal_defs.hpp>
//
// // NOLINTBEGIN(readability-identifier-naming)
//
// // ──────────────────────────────────────────────────────────────────────────────
// // Trait extraction — same approach as core property_defs.hpp,
// // adapted to use trailing-underscore member naming (name_).
// // ──────────────────────────────────────────────────────────────────────────────
//
// #define __NXP2_SELECT_TRAIT(...) __NXP2_SELECT_TRAIT_(__VA_ARGS__)
// #define __NXP2_SELECT_TRAIT_(name, tk_name, tk_val) __NXP2_SELECT_TRAIT_##name##_FROM_##tk_name(tk_val)
//
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_TYPE(t)       t
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_TYPE_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_NAME_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_NAME(n)       n
// #define __NXP2_SELECT_TRAIT_NAME_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_NAME_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_READ_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_READ_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_READ_FROM_READ(r)       r
// #define __NXP2_SELECT_TRAIT_READ_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_READ_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_READ_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_READ_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_READ_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_WRITE(w)     w
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_WRITE_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_NOTIFY(n)   n
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_NOTIFY_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_DEFAULT(d) d
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_DEFAULT_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_RESET_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_RESET_FROM_RESET(r)     r
// #define __NXP2_SELECT_TRAIT_RESET_FROM_MEMBER(m)
//
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_TYPE(t)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_NAME(n)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_READ(r)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_WRITE(w)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_NOTIFY(n)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_DEFAULT(d)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_RESET(r)
// #define __NXP2_SELECT_TRAIT_MEMBER_FROM_MEMBER(m)   m
//
// // Token-kind tags
// #define __NXP2_EXPAND_TK_TYPE(t)    TYPE,    t
// #define __NXP2_EXPAND_TK_NAME(n)    NAME,    n
// #define __NXP2_EXPAND_TK_READ(r)    READ,    r
// #define __NXP2_EXPAND_TK_WRITE(w)   WRITE,   w
// #define __NXP2_EXPAND_TK_NOTIFY(n)  NOTIFY,  n
// #define __NXP2_EXPAND_TK_DEFAULT(d) DEFAULT, d
// #define __NXP2_EXPAND_TK_RESET(r)   RESET,   r
// #define __NXP2_EXPAND_TK_MEMBER(m)  MEMBER,  m
//
// #define __NXP2_TRAIT_TYPE_(_)    _TK_TYPE(_)
// #define __NXP2_TRAIT_NAME_(_)    _TK_NAME(_)
// #define __NXP2_TRAIT_READ_(_)    _TK_READ(_)
// #define __NXP2_TRAIT_WRITE_(_)   _TK_WRITE(_)
// #define __NXP2_TRAIT_NOTIFY_(_)  _TK_NOTIFY(_)
// #define __NXP2_TRAIT_DEFAULT_(_) _TK_DEFAULT(_)
// #define __NXP2_TRAIT_RESET_(_)   _TK_RESET(_)
// #define __NXP2_TRAIT_MEMBER_(_)  _TK_MEMBER(_)
//
// #define __NXP2_TRAIT_TYPE    __NXP2_TRAIT_TYPE_(
// #define __NXP2_TRAIT_NAME    __NXP2_TRAIT_NAME_(
// #define __NXP2_TRAIT_READ    __NXP2_TRAIT_READ_(
// #define __NXP2_TRAIT_WRITE   __NXP2_TRAIT_WRITE_(
// #define __NXP2_TRAIT_NOTIFY  __NXP2_TRAIT_NOTIFY_(
// #define __NXP2_TRAIT_DEFAULT __NXP2_TRAIT_DEFAULT_(
// #define __NXP2_TRAIT_RESET   __NXP2_TRAIT_RESET_(
// #define __NXP2_TRAIT_MEMBER  __NXP2_TRAIT_MEMBER_(
//
// #define __NXP2_IT_CONVERT_TRAIT_TOKEN(n, ...) \
//     NX_CONCAT(__NXP2_TRAIT_, NX_CHOOSE(n, , __VA_ARGS__)) )
//
// #define __NXP2_IT_FIND_TRAIT(n, trait, ...) \
//     __NXP2_SELECT_TRAIT(trait, NX_EXPAND(NX_CONCAT(__NXP2_EXPAND_TK_, NX_CHOOSE(n, , __VA_ARGS__))))
//
// #define __NXP2_FIND_TRAIT_TOKEN_(trait, ...) \
//     NX_REPEAT_NO_COMMA(NX_ARGS_COUNT(__VA_ARGS__), __NXP2_IT_FIND_TRAIT, trait, __VA_ARGS__)
//
// #define __NXP2_FIND_TRAIT_TOKEN(...) __NXP2_FIND_TRAIT_TOKEN_(__VA_ARGS__)
//
// #define __NXP2_HAS_TRAIT_TOKEN_(trait, ...) \
//     NX_APPLY(NX_HAS_ARGS, NX_EXPAND(__NXP2_FIND_TRAIT_TOKEN(trait, __VA_ARGS__)))
// #define __NXP2_HAS_TRAIT_TOKEN(...) __NXP2_HAS_TRAIT_TOKEN_(__VA_ARGS__)
//
// #define __NXP2_CONVERT_TO_TRAIT_TOKENS(...) \
//     NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NXP2_IT_CONVERT_TRAIT_TOKEN, __VA_ARGS__)
//
// #define __NXP2_FIND_TRAIT(trait, ...) \
//     __NXP2_FIND_TRAIT_TOKEN(trait, __NXP2_CONVERT_TO_TRAIT_TOKENS(__VA_ARGS__))
//
// #define __NXP2_HAS_TRAIT(trait, ...) \
//     __NXP2_HAS_TRAIT_TOKEN(trait, __NXP2_CONVERT_TO_TRAIT_TOKENS(__VA_ARGS__))
//
// // ──────────────────────────────────────────────────────────────────────────────
// // Member name — trailing underscore convention: `count` → `count_`
// // ──────────────────────────────────────────────────────────────────────────────
//
// #define __NXP2_NAME_TO_MEMBER(name) NX_CONCAT(name, _)
//
// #define __NXP2_FIND_NAME_0(...) __NXP2_FIND_TRAIT(MEMBER, __VA_ARGS__)
// #define __NXP2_FIND_NAME_1(...) __NXP2_FIND_TRAIT(NAME,   __VA_ARGS__)
// #define __NXP2_FIND_NAME(...) \
//     NX_CONCAT(__NXP2_FIND_NAME_, __NXP2_HAS_TRAIT(NAME, __VA_ARGS__))(__VA_ARGS__)
//
// // ──────────────────────────────────────────────────────────────────────────────
// // Code generation
// // ──────────────────────────────────────────────────────────────────────────────
//
// // Member declaration
// #define __NXP2_DECL_0_0(type, name, ...) \
//     type __NXP2_NAME_TO_MEMBER(name);
// #define __NXP2_DECL_0_1(type, name, default_val) \
//     type __NXP2_NAME_TO_MEMBER(name) = default_val;
// #define __NXP2_DECL_0(type, name, default_val, has_default) \
//     NX_CONCAT(__NXP2_DECL_0_, has_default)(type, name, default_val)
// #define __NXP2_DECL_1(...) NX_CONSUME(__VA_ARGS__)  // MEMBER: don't generate
// #define __NXP2_DECL(type, name, has_default, default_val, is_member) \
//     private:                                                          \
//     NX_CONCAT(__NXP2_DECL_, is_member)(type, name, default_val, has_default) \
//     public:
//
// // Getter
// #define __NXP2_GETTER_0(...) NX_CONSUME(__VA_ARGS__)
// #define __NXP2_GETTER_1(type, name, getter_name) \
//     NX_NODISCARD type getter_name() const { return __NXP2_NAME_TO_MEMBER(name); }
// #define __NXP2_GETTER(type, name, has_getter, getter_name) \
//     NX_CONCAT(__NXP2_GETTER_, has_getter)(type, name, getter_name)
//
// // Notify signal
// #define __NXP2_NOTIFY_0(...) NX_CONSUME(__VA_ARGS__)
// #define __NXP2_NOTIFY_1(type, name, notify_name) \
//     NX_SIGNAL(notify_name, type)
// #define __NXP2_NOTIFY(type, name, has_notify, notify_name) \
//     NX_CONCAT(__NXP2_NOTIFY_, has_notify)(type, name, notify_name)
//
// // Setter — without notify
// #define __NXP2_SETTER_1_0(type, name, setter_name, ...) \
//     void setter_name(const type & _new)                  \
//     {                                                    \
//         __NXP2_NAME_TO_MEMBER(name) = _new;              \
//     }
// // Setter — with notify
// #define __NXP2_SETTER_1_1(type, name, setter_name, notify_name) \
//     void setter_name(const type & _new)                          \
//     {                                                            \
//         if (__NXP2_NAME_TO_MEMBER(name) == _new)                 \
//             return;                                              \
//         __NXP2_NAME_TO_MEMBER(name) = _new;                      \
//         NX_EMIT(notify_name, _new);                              \
//     }
// #define __NXP2_SETTER_0(...) NX_CONSUME(__VA_ARGS__)
// #define __NXP2_SETTER_1(type, name, setter_name, has_notify, notify_name) \
//     NX_CONCAT(__NXP2_SETTER_1_, has_notify)(type, name, setter_name, notify_name)
// #define __NXP2_SETTER(type, name, has_setter, setter_name, has_notify, notify_name) \
//     NX_CONCAT(__NXP2_SETTER_, has_setter)(type, name, setter_name, has_notify, notify_name)
//
// // Reset method
// #define __NXP2_RESET_VAL_0(type, ...)   type{}
// #define __NXP2_RESET_VAL_1(type, dval)  dval
// #define __NXP2_RESET_VAL(type, has_def, dval) \
//     NX_CONCAT(__NXP2_RESET_VAL_, has_def)(type, dval)
//
// #define __NXP2_RESET_NOTIFY_0(...) NX_CONSUME(__VA_ARGS__)
// #define __NXP2_RESET_NOTIFY_1(nval, notify_name) NX_EMIT(notify_name, nval);
// #define __NXP2_RESET_NOTIFY(has_notify, nval, notify_name) \
//     NX_CONCAT(__NXP2_RESET_NOTIFY_, has_notify)(nval, notify_name)
//
// #define __NXP2_RESET_0(...) NX_CONSUME(__VA_ARGS__)
// #define __NXP2_RESET_1(type, name, reset_name, has_def, dval, has_notify, notify_name) \
//     void reset_name()                                                                    \
//     {                                                                                    \
//         const auto _dv = __NXP2_RESET_VAL(type, has_def, dval);                        \
//         if (__NXP2_NAME_TO_MEMBER(name) == _dv) return;                                 \
//         __NXP2_NAME_TO_MEMBER(name) = _dv;                                              \
//         __NXP2_RESET_NOTIFY(has_notify, _dv, notify_name)                               \
//     }
// #define __NXP2_RESET(type, name, has_reset, reset_name, has_def, dval, has_notify, notify_name) \
//     NX_CONCAT(__NXP2_RESET_, has_reset)(type, name, reset_name, has_def, dval, has_notify, notify_name)
//
// // ── Entry point ───────────────────────────────────────────────────────────────
//
// #define __NXP2_CREATE_(type, name, is_member,               \
//                        has_getter, getter_name,              \
//                        has_setter, setter_name,              \
//                        has_notify, notify_name,              \
//                        has_default, default_val,             \
//                        has_reset,  reset_name)               \
//     __NXP2_DECL(type, name, has_default, default_val, is_member) \
//     __NXP2_GETTER(type, name, has_getter, getter_name)            \
//     __NXP2_NOTIFY(type, name, has_notify, notify_name)            \
//     __NXP2_SETTER(type, name, has_setter, setter_name, has_notify, notify_name) \
//     __NXP2_RESET(type, name, has_reset, reset_name, has_default, default_val, has_notify, notify_name)
//
// #define __NXP2_CREATE(...)                               \
//     __NXP2_CREATE_(                                      \
//         __NXP2_FIND_TRAIT(TYPE, __VA_ARGS__),            \
//         __NXP2_FIND_NAME(__VA_ARGS__),                   \
//         __NXP2_HAS_TRAIT(MEMBER,  __VA_ARGS__),          \
//         __NXP2_HAS_TRAIT(READ,    __VA_ARGS__), __NXP2_FIND_TRAIT(READ,    __VA_ARGS__), \
//         __NXP2_HAS_TRAIT(WRITE,   __VA_ARGS__), __NXP2_FIND_TRAIT(WRITE,   __VA_ARGS__), \
//         __NXP2_HAS_TRAIT(NOTIFY,  __VA_ARGS__), __NXP2_FIND_TRAIT(NOTIFY,  __VA_ARGS__), \
//         __NXP2_HAS_TRAIT(DEFAULT, __VA_ARGS__), __NXP2_FIND_TRAIT(DEFAULT, __VA_ARGS__), \
//         __NXP2_HAS_TRAIT(RESET,   __VA_ARGS__), __NXP2_FIND_TRAIT(RESET,   __VA_ARGS__))
//
// #define __NXP2_CHECK_1_0(...) \
//     static_assert(false, "NX_PROPERTY requires TYPE and NAME (or MEMBER) traits");
// #define __NXP2_CHECK_1_1(...) __NXP2_CREATE(__VA_ARGS__)
//
// #define __NXP2_HAS_TYPE_AND_NAME_OR_MEMBER(...) \
//     NX_EXPAND(NX_XOR(                                                     \
//         NX_AND(__NXP2_HAS_TRAIT(TYPE, __VA_ARGS__),                       \
//                __NXP2_HAS_TRAIT(NAME, __VA_ARGS__)),                      \
//         NX_AND(__NXP2_HAS_TRAIT(TYPE, __VA_ARGS__),                       \
//                __NXP2_HAS_TRAIT(MEMBER, __VA_ARGS__))))
//
// #define __NXP2(...) \
//     NX_CONCAT(__NXP2_CHECK_1_, __NXP2_HAS_TYPE_AND_NAME_OR_MEMBER(__VA_ARGS__))(__VA_ARGS__)
//
// /// NX_PROPERTY(TYPE type, NAME name [, READ r] [, WRITE w] [, NOTIFY n] [, DEFAULT d] [, RESET r])
// #define NX_PROPERTY(...) __NXP2(__VA_ARGS__)
//
// // NOLINTEND(readability-identifier-naming)

//
// Created by nexie on 25.11.2025.
//

#ifndef PROPERTY_DEFS_HPP
#define PROPERTY_DEFS_HPP

#include <nx/macro/repeat.hpp>
#include <nx/macro/args/count.hpp>
#include <nx/macro/util/choose.hpp>
#include <nx/macro/logic.hpp>

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
        NX_XOR( \
            NX_AND(__NX_PROPERTY_HAS_TRAIT(TYPE, __VA_ARGS__), __NX_PROPERTY_HAS_TRAIT(NAME, __VA_ARGS__)), \
            NX_AND(__NX_PROPERTY_HAS_TRAIT(TYPE, __VA_ARGS__), __NX_PROPERTY_HAS_TRAIT(MEMBER, __VA_ARGS__)) \
        ) \
    )

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
    private: \
    NX_CONCAT(__NX_PROPERTY_CREATE_DECL_, is_member)(type, name, default_value, has_default) \
    public:

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

namespace nx
{

}



#endif //PROPERTY_DEFS_HPP

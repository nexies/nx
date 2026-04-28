//
// Created by nexie on 4/21/2026.
//

#ifndef NX_PROPERTY_DEFS2_HPP
#define NX_PROPERTY_DEFS2_HPP

/// NX_PROPERTY macro v2
/// =====================
///
/// Purpose:
///   Register a "property" of an enclosing class in the meta-property system
///   (getter, setter, reset, notify signal). Optionally generate storage fields
///   and/or accessor methods directly on the class.
///
/// Syntax:
///   NX_PROPERTY(name, <keywords...>)
///
///
/// ---------------------------------------------------------------------------
/// STORAGE KEYWORDS  (mutually exclusive — at most one)
/// ---------------------------------------------------------------------------
///
///   TYPE <type>
///     Declares the property type and instructs the macro to generate a new
///     data member:
///         <type> m_property_<name>;
///     If DEFAULT <value> is also present, the field gets that initializer.
///     If CONST is also present, the field is declared const (only makes sense
///     with DEFAULT or a constructor-time initializer).
///
///   MEMBER <member>
///     Points to an already-existing data member that holds the value.
///     The property type is inferred via data_member_trait<decltype(&T::<member>)>
///     (strips ClassType::* to yield the plain MemberType).
///     No new field is generated.
///
///   (neither TYPE nor MEMBER)
///     No storage is created or assumed. The property is "computed" — its value
///     is produced entirely by the accessor methods supplied via READ / WRITE.
///     In this case READ (and/or WRITE) MUST provide an explicit method name.
///
///
/// ---------------------------------------------------------------------------
/// ACCESSOR KEYWORDS  (independent, each optional)
/// ---------------------------------------------------------------------------
///
///   READ [method]
///
///     READ  (bare, no argument):
///       Generate  auto get_<name>() const;  that reads from the storage field.
///       Requires TYPE or MEMBER to be present.
///       Registers &T::get_<name> in the meta-property system as getter.
///
///     READ <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as getter instead.
///       When neither TYPE nor MEMBER is present, the property type is inferred
///       from <method>'s return type.
///
///     (READ absent) + TYPE or MEMBER present:
///       No method is generated on the class, but a primitive lambda getter
///           [](const T* s) -> auto { return s->m_property_<name>; }
///       is registered in the meta-system for direct field access.
///
///     (READ absent) + no TYPE, no MEMBER:
///       No getter is registered at all.
///
///
///   WRITE [method]
///
///     WRITE  (bare, no argument):
///       Generate  void set_<name>(const <type>& value);
///       Body: assigns value to the storage field; emits NOTIFY signal if one
///       is registered.  If NOTIFY is absent, just assigns — method still exists.
///       Requires TYPE or MEMBER to be present.
///       Registers &T::set_<name> in the meta-property system as setter.
///       Incompatible with CONST — compile error if both are present.
///
///     WRITE <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as setter instead.
///       Incompatible with CONST.
///
///     (WRITE absent) + TYPE or MEMBER present:
///       A primitive lambda setter is registered in the meta-system.
///       No method generated on the class.
///       Still incompatible with CONST — CONST suppresses even primitive setter.
///
///     (WRITE absent) + no TYPE, no MEMBER:
///       No setter registered.
///
///
///   RESET [method]
///
///     RESET  (bare, no argument):
///       Generate  void reset_<name>();
///       Body: assigns the DEFAULT value (if given) to the storage field, or
///       value-initialises it (<type>{}).  Emits NOTIFY if registered.
///       If NOTIFY is absent, just resets — method still exists.
///       Requires TYPE or MEMBER to be present.
///       Registers &T::reset_<name> in the meta-property system.
///       Incompatible with CONST — compile error if both are present.
///
///     RESET <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as reset instead.
///       Incompatible with CONST.
///
///     (RESET absent):
///       No reset registered in the meta-system.
///
///
///   NOTIFY [signal]
///
///     NOTIFY  (bare):
///       Generate  NX_SIGNAL(<name>_changed);  on the class and register it.
///       The signal carries the new value as its argument.
///
///     NOTIFY <signal>:
///       Use existing signal <signal> and register it.
///
///     (NOTIFY absent):
///       No notify signal is wired up.  Generated set_<name> / reset_<name>
///       still exist and work — they just do not emit anything.
///
///
///   DEFAULT <value>
///     Only valid together with TYPE.
///     Sets the field's initialiser:
///         <type> m_property_<name>{ <value> };
///     Also used as the reset value inside a generated reset_<name>().
///     If RESET is present but DEFAULT is absent, reset_<name>() value-initialises
///     the field (<type>{}).
///
///
/// ---------------------------------------------------------------------------
/// QUALIFIER KEYWORDS  (flags, no arguments)
/// ---------------------------------------------------------------------------
///
///   CONST
///     Marks the property as immutable — its value is set once (at construction)
///     and cannot be changed afterwards through the meta-property system.
///     Effect:
///       - WRITE / RESET are disallowed (compile error if combined with CONST).
///       - No setter or reset is registered in the meta-system.
///       - If TYPE is present, the generated field is declared const.
///         (Initialise it via a constructor member-initialiser list or DEFAULT.)
///       - The getter is still generated/registered normally.
///     Use for identity-like properties: object ID, creation timestamp, etc.
///
///
///   FINAL                                           [UNDER CONSTRUCTION]
///     Marks the property as non-overridable in derived classes.
///     If a derived class declares NX_PROPERTY with the same name, it is a
///     compile error (or at least a meta-system assertion failure).
///     Mechanism TBD — candidates:
///       a) Inject a deleted / final-tagged sentinel type into the class so
///          that a re-declaration in a subclass causes a redefinition error.
///       b) Record the "final" flag in the meta-descriptor and assert at
///          registration time that no base class already owns this name as final.
///     Either way the flag must be recorded in the meta-descriptor so that
///     tooling and runtime introspection can honour it.
///
///
///   MUTABLE                                         [UNDER CONSTRUCTION]
///     Intended to allow mutation of the property even through a const reference
///     to the owning object (analogous to the C++ `mutable` storage-class).
///     Possible use-case: a lazily-computed cached value that is conceptually
///     const from the outside but needs to be refreshed internally.
///     Interaction with CONST: likely disallowed (contradictory).
///     Exact semantics and code-generation rules TBD.
///
///
/// ---------------------------------------------------------------------------
/// SUMMARY TABLE  (getter column; WRITE/RESET follow the same pattern)
/// ---------------------------------------------------------------------------
///
///  Storage   | READ form    | Field gen? | Method gen?  | Meta getter
///  ----------|--------------|------------|--------------|---------------------
///  TYPE T    | (absent)     | yes        | no           | primitive lambda
///  TYPE T    | READ         | yes        | get_<name>() | &T::get_<name>
///  TYPE T    | READ foo     | yes        | no           | &T::foo
///  MEMBER m  | (absent)     | no         | no           | primitive lambda
///  MEMBER m  | READ         | no         | get_<name>() | &T::get_<name>
///  MEMBER m  | READ foo     | no         | no           | &T::foo
///  (none)    | READ foo     | no         | no           | &T::foo
///  (none)    | READ (bare)  | —          | INVALID      | needs storage
///
///
/// ---------------------------------------------------------------------------
/// EXAMPLES
/// ---------------------------------------------------------------------------
///
///
/// Example 1 — Full auto-generation (most common case)
///
///   NX_PROPERTY(id, TYPE int, READ, WRITE, NOTIFY, RESET, DEFAULT 0)
///
///   // Generates:
///   //   int m_property_id{ 0 };
///   //   int  get_id()  const              { return m_property_id; }
///   //   void set_id(const int& value)     { m_property_id = value; emit id_changed(value); }
///   //   void reset_id()                   { m_property_id = 0;     emit id_changed(0); }
///   //   NX_SIGNAL(id_changed, int)
///   //
///   // Meta: getter=&T::get_id, setter=&T::set_id, reset=&T::reset_id, notify=&T::id_changed
///
///
/// Example 2 — Custom storage, auto accessor methods, existing notify signal
///
///   NX_PROPERTY(name, MEMBER m_name_, READ, WRITE, NOTIFY name_changed)
///
///   // Generates:
///   //   auto get_name() const                         { return m_name_; }
///   //   void set_name(const decltype(m_name_)& value) { m_name_ = value; emit name_changed(value); }
///   //
///   // Uses existing: m_name_, name_changed
///   // Meta: getter=&T::get_name, setter=&T::set_name, notify=&T::name_changed
///
///
/// Example 3 — Computed property, no storage
///
///   NX_PROPERTY(full_name, READ get_full_name)
///
///   // Uses existing: get_full_name()
///   // Type inferred from get_full_name() return type.
///   // Meta: getter=&T::get_full_name.  No setter, no field.
///
///
/// Example 4 — Minimal: expose existing field to meta-system only
///
///   NX_PROPERTY(score, MEMBER m_score)
///
///   // Uses existing: m_score  (type via data_member_trait)
///   // Meta: primitive lambda getter + setter for m_score.
///   // No methods added to the class.
///
///
/// Example 5 — Mixed: generated getter, complex existing setter
///
///   NX_PROPERTY(health, MEMBER m_health, READ, WRITE apply_damage_model, NOTIFY health_changed)
///
///   // Generates: auto get_health() const { return m_health; }
///   // Uses existing: apply_damage_model(), health_changed
///   // Meta: getter=&T::get_health, setter=&T::apply_damage_model, notify=&T::health_changed
///
///
/// Example 6 — Read-only field (no write in meta-system)
///
///   NX_PROPERTY(token, TYPE std::string, READ, NOTIFY token_changed)
///
///   // Generates:
///   //   std::string m_property_token;
///   //   std::string get_token() const { return m_property_token; }
///   //   NX_SIGNAL(token_changed, std::string)
///   //
///   // Meta: getter=&T::get_token, notify=&T::token_changed.  No setter registered.
///
///
/// Example 7 — CONST: immutable property, set at construction only
///
///   NX_PROPERTY(uid, TYPE std::string, READ, CONST)
///
///   // Generates:
///   //   const std::string m_property_uid;   // must be set in ctor init-list
///   //   std::string get_uid() const { return m_property_uid; }
///   //
///   // Meta: getter=&T::get_uid.  No setter or reset — CONST forbids them.
///
///
/// Example 8 — Setter without NOTIFY (method still exists, just doesn't emit)
///
///   NX_PROPERTY(debug_level, TYPE int, READ, WRITE, DEFAULT 0)
///
///   // Generates:
///   //   int m_property_debug_level{ 0 };
///   //   int  get_debug_level() const          { return m_property_debug_level; }
///   //   void set_debug_level(const int& value) { m_property_debug_level = value; }
///   //                                          // no emit — NOTIFY absent
///   // Meta: getter=&T::get_debug_level, setter=&T::set_debug_level
///
///
/// Example 9 — FINAL: prevent derived classes from overriding (under construction)
///
///   NX_PROPERTY(object_type, TYPE std::string, READ, CONST, FINAL)
///
///   // Same generation as CONST example above, plus:
///   // Meta-descriptor records this property as final.
///   // A subclass declaring NX_PROPERTY(object_type, ...) triggers a compile
///   // error (or runtime assertion during meta-registration).
///
/// ---------------------------------------------------------------------------

# pragma region NX_PROPERTY_v2

#include <nx/macro/repeat.hpp>
#include <nx/macro/numeric.hpp>
#include <nx/macro/tuple/tuple.hpp>
#include <nx/macro/args/token.hpp>
#include <nx/core2/detail/signal_defs.hpp>

/// ---=== PARAMETERS DECLARATION ===---

# define _nxpv2_prefix _nxpv2_
# define _nxpv2(arg) _nx_concat_2(_nxpv2_, arg)

# define _nxpv2_TYPE            0

# define _nxpv2_MEMBER          1

# define _nxpv2_READ            2
# define _nxpv2_GET             2

# define _nxpv2_WRITE           3
# define _nxpv2_SET             3

# define _nxpv2_RESET           4

# define _nxpv2_NOTIFY          5

# define _nxpv2_DEFAULT         6

# define _nxpv2_CONST           7

# define _nxpv2_FINAL           8

# define _nxpv2_MUTABLE         9

# define _nxpv2_READONLY        10

# define _nxpv2_WRITEONLY       11

/// ---=== ARGUMENTS STORAGE SYSTEM ===---

/* TYPE */
/* STORED/MEMBER */
/* READ/GET */
/* WRITE/SET */
/* RESET */
/* NOTIFY */
/* DEFAULT */
/* CONST */
/* FINAL */
/* MUTABLE */
/* READONLY */
/* WRITEONLY */
# define _nxpv2_default_args                    \
    _nx_tuple(                                  \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ()),            \
        (0, ())             \
    )

# define _nxpv2_bare_exists_arg_1(...) _nxpv2_bare_exists_arg(__VA_ARGS__)
# define _nxpv2_bare_exists_arg(count, ...) count
# define _nxpv2_bare_value_tup_arg(count, val) val

# define _nxpv2_tokenize(arg) \
    _nx_apply(_nx_args_tokenize, _nx_concat_2(_nxpv2_, arg))

// # define _nxpv2_args_add(args, token)                                                           \
//     _nx_logic_if(_nx_args_token_has_value(token))(                                              \
//         _nx_tuple_set(args, _nx_args_token_name(token), (1, (_nx_args_token_value(token)))),     \
//         _nx_tuple_set(args, _nx_args_token_name(token), (1, ()))                                 \
//     )

# define _nxpv2_args_add(args, token)                                                           \
        _nx_tuple_set(args, \
            _nx_args_token_name(token),    \
            (1, ( _nx_args_token_value(token) ) )   \
        )


# define _nxpv2_sort_args_op_d(d, res, arg, ...)                                                \
    _nxpv2_args_add(res, _nxpv2_tokenize(arg)) _nx_append_args(__VA_ARGS__)

# define _nxpv2_sort_args_cond_d(d, res, ...) \
    _nx_args_not_empty(__VA_ARGS__)

# define _nxpv2_sort_args_res_d(d, res, ...) \
    res

# define _nxpv2_sort_args_d(d, ...) \
    _nx_while_d(d)( \
        _nxpv2_sort_args_cond_d, \
        _nxpv2_sort_args_op_d, \
        _nxpv2_sort_args_res_d, \
        _nxpv2_default_args _nx_append_args(__VA_ARGS__) \
    )

# define _nxpv2_get_arg(args, name) \
    _nx_tuple_get(_nxpv2_##name, args)

# define _nxpv2_exists_arg(args, name) \
    _nxpv2_bare_exists_arg_1( _nx_tuple_unpack(_nxpv2_get_arg(args, name)) )

# define _nxpv2_get_value_tup(args, name) \
    _nxpv2_bare_value_tup_arg _nxpv2_get_arg(args, name)

# define _nxpv2_has_value(args, name) \
    _nx_apply(_nx_tuple_not_empty, _nxpv2_get_value_tup(args, name))

# define _nxpv2_get_value(args, name)                                    \
    _nx_logic_if(_nxpv2_has_value(args, name))(                          \
        _nx_apply(_nx_tuple_get, 0, _nxpv2_get_value_tup(args, name)),   \
        _nx_empty()                                                      \
    )

// # define _tmp_nxpv2_args \
//     _nxpv2_sort_args_d(0, TYPE float, MEMBER value, READ, WRITE, NOTIFY value_changed, DEFAULT 123)

/// ---=== DEFAULT PARAMETERS PREFIXES AND KEYWORDS ===---

# define _nxpv2_add_prefix(prefix, name) \
    _nx_concat_3(prefix, _, name)

# define _nxpv2_add_postfix(name, postfix) \
    _nx_concat_3(name, _, postfix)

# define _nxpv2_default_read_prefix get
# define _nxpv2_default_write_prefix set
# define _nxpv2_default_reset_prefix reset
# define _nxpv2_default_notify_postfix changed
# define _nxpv2_default_member_prefix m_property

# define _nxpv2_keyword_const const
# define _nxpv2_keyword_mutable mutable

/// ---=== PARAMETERS ACCESSORS ===---

/// TYPE

// TYPE always has a value when present (e.g. TYPE int → value = int)
# define _nxpv2_get_type(args)                                   \
    _nxpv2_get_value(args, TYPE)


/// MEMBER

// MEMBER parameter exists and has value
# define _nxpv2_get_existing_member(name, args)                 \
    _nxpv2_get_value(args, MEMBER)

// MEMBER parameter may not be present
# define _nxpv2_get_member(name, args)                              \
    _nx_logic_if(_nxpv2_has_value(args, MEMBER))(                  \
        _nxpv2_get_existing_member(name, args),                    \
        _nxpv2_add_prefix(_nxpv2_default_member_prefix, name)      \
    )

/// READ

# define _nxpv2_default_read_value(name)                        \
    _nxpv2_add_prefix(_nxpv2_default_read_prefix, name)

//READ parameter exists, but may not have a value
# define _nxpv2_get_existing_read(name, args)                   \
    _nx_logic_if(_nxpv2_has_value(args, READ))(                 \
        _nxpv2_get_value(args, READ),                           \
        _nxpv2_default_read_value(name)                         \
    )

// READ parameter may not exist at this point
# define _nxpv2_get_read(name, args)                            \
    _nx_logic_if(_nxpv2_exists_arg(args, READ)) (               \
        _nxpv2_get_existing_read(name, args),                   \
        _nx_empty()                                             \
    )


/// WRITE

# define _nxpv2_default_write_value(name)                        \
    _nxpv2_add_prefix(_nxpv2_default_write_prefix, name)

//WRITE parameter exists, but may not have a value
# define _nxpv2_get_existing_write(name, args)                   \
    _nx_logic_if(_nxpv2_has_value(args, WRITE))(                 \
        _nxpv2_get_value(args, WRITE),                           \
        _nxpv2_default_write_value(name)                         \
    )

// WRITE parameter may not exist at this point
# define _nxpv2_get_write(name, args)                            \
    _nx_logic_if(_nxpv2_exists_arg(args, WRITE)) (               \
        _nxpv2_get_existing_write(name, args),                   \
        _nx_empty()                                             \
    )


/// RESET

# define _nxpv2_default_reset_value(name)                        \
    _nxpv2_add_prefix(_nxpv2_default_reset_prefix, name)

// RESET parameter exists, but may not have a value
# define _nxpv2_get_existing_reset(name, args)                   \
    _nx_logic_if(_nxpv2_has_value(args, RESET))(                 \
        _nxpv2_get_value(args, RESET),                           \
        _nxpv2_default_reset_value(name)                         \
    )

// RESET parameter may not exist
# define _nxpv2_get_reset(name, args)                            \
    _nx_logic_if(_nxpv2_exists_arg(args, RESET))(                \
        _nxpv2_get_existing_reset(name, args),                   \
        _nx_empty()                                              \
    )


/// NOTIFY

# define _nxpv2_default_notify_value(name)                       \
    _nxpv2_add_postfix(name, _nxpv2_default_notify_postfix)

// NOTIFY parameter exists, but may not have a value (bare NOTIFY → auto-generate signal name)
# define _nxpv2_get_existing_notify(name, args)                  \
    _nx_logic_if(_nxpv2_has_value(args, NOTIFY))(                \
        _nxpv2_get_value(args, NOTIFY),                          \
        _nxpv2_default_notify_value(name)                        \
    )

// NOTIFY parameter may not exist
# define _nxpv2_get_notify(name, args)                           \
    _nx_logic_if(_nxpv2_exists_arg(args, NOTIFY))(               \
        _nxpv2_get_existing_notify(name, args),                  \
        _nx_empty()                                              \
    )


/// DEFAULT

// DEFAULT always has a value: either the user-supplied one or {} from _nxpv2_default_args.
// Use _nxpv2_exists_arg to check if the user explicitly passed DEFAULT.
# define _nxpv2_get_default(args)                                \
    _nxpv2_get_value(args, DEFAULT)

# define _nxpv2_get_default_bare_value(args)                              \
    _nx_logic_if(_nxpv2_has_value(args, DEFAULT))(                        \
        _nx_apply(_nx_tuple_get, 0, _nxpv2_get_value_tup(args, DEFAULT)), \
        _nx_expand( {} )                                                  \
    )

/// CONST

// CONST is a flag — no value, just presence check
# define _nxpv2_is_const(args)                                   \
    _nxpv2_exists_arg(args, CONST)

// get `const` modificator
# define _nxpv2_get_const(args)                                  \
    _nx_logic_if(_nxpv2_is_const(args)) (                        \
        _nxpv2_keyword_const,                                    \
        _nx_empty()                                              \
    )

/// FINAL

// FINAL is a flag — no value, just presence check
# define _nxpv2_is_final(args)                                   \
    _nxpv2_exists_arg(args, FINAL)


/// MUTABLE

// MUTABLE is a flag — no value, just presence check
# define _nxpv2_is_mutable(args)                                 \
    _nxpv2_exists_arg(args, MUTABLE)

// get `mutable` modificator
# define _nxpv2_get_mutable(args)                                \
    _nx_logic_if(_nxpv2_is_mutable(args)) (                      \
        _nxpv2_keyword_mutable,                                  \
        _nx_empty()                                              \
    )

/// ---=== CODE GENERATION ===---

// ----------------------------------------------------------------
// Type helpers for case MEMBER (no explicit TYPE):
//   - strip cv + ref from decltype of the member
//   - infer from return type of a const getter (case computed, no storage)
// ----------------------------------------------------------------

# include <type_traits>

# define _nxpv2_member_type(member_name)                                    \
    std::remove_cv_t<std::remove_reference_t<                               \
        decltype(std::declval<_nx_self_t &>().member_name)>>

# define _nxpv2_computed_type(read_method)                                  \
    std::remove_cv_t<std::remove_reference_t<                               \
        decltype(std::declval<const _nx_self_t &>().read_method())>>

// ----------------------------------------------------------------
// Gen: field declaration   (used only when TYPE is present)
//
//   [const] [mutable] <type> m_property_<name> { <default> };
// ----------------------------------------------------------------

# define _nxpv2_gen_field_decl(prop_name, args)                             \
    _nxpv2_get_const(args)                                                  \
    _nxpv2_get_mutable(args)                                                \
    _nxpv2_get_type(args)                                                   \
    _nxpv2_add_prefix(_nxpv2_default_member_prefix, prop_name)              \
    { _nxpv2_get_default(args) };

// ----------------------------------------------------------------
// Gen: getter method
//
//   bare READ  → auto get_<name>() const { return field_ref; }
//   READ meth  → nothing  (existing method — registered in meta only)
//   no READ    → nothing
// ----------------------------------------------------------------

# define _nxpv2_gen_getter_method(prop_name, field_ref)                     \
    auto _nxpv2_add_prefix(_nxpv2_default_read_prefix, prop_name)() const -> decltype(field_ref){ \
        return field_ref;                                                    \
    }

# define _nxpv2_gen_read_dispatch(prop_name, field_ref, args)               \
    _nx_logic_if(_nxpv2_exists_arg(args, READ))(                            \
        _nx_logic_if(_nxpv2_has_value(args, READ))(                         \
            _nx_empty(),                                                     \
            _nxpv2_gen_getter_method(prop_name, field_ref)                  \
        ),                                                                   \
        _nx_empty()                                                          \
    )

// ----------------------------------------------------------------
// Gen: NOTIFY signal
//
//   bare NOTIFY  → NX_SIGNAL(<name>_changed)   — no argument: avoids copies of
//                                                 complex types and is safe across
//                                                 threads (observer calls getter).
//   NOTIFY sig   → nothing  (existing signal — registered in meta only)
//   no NOTIFY    → nothing
// ----------------------------------------------------------------

# define _nxpv2_gen_signal(prop_name)                                       \
    NX_SIGNAL(_nxpv2_add_postfix(prop_name, _nxpv2_default_notify_postfix))

# define _nxpv2_gen_notify_dispatch(prop_name, type_expr, args)             \
    _nx_logic_if(_nxpv2_exists_arg(args, NOTIFY))(                          \
        _nx_logic_if(_nxpv2_has_value(args, NOTIFY))(                       \
            _nx_empty(),                                                     \
            _nx_logic_if(_nxpv2_exists_arg(args, MEMBER))(                  \
                static_assert(false,                                         \
                    "NX_PROPERTY: bare NOTIFY with MEMBER is not supported " \
                    "because the member type cannot be deduced while the "   \
                    "class is incomplete. Pre-declare the signal and use "   \
                    "NOTIFY <signal_name> instead."),                        \
                _nxpv2_gen_signal(prop_name)                                \
            )                                                                \
        ),                                                                   \
        _nx_empty()                                                          \
    )

// ----------------------------------------------------------------
// Gen: setter method
//
//   bare WRITE + not CONST → void set_<name>(const type& _value) { ... }
//   WRITE meth / CONST / no WRITE → nothing
//   If NOTIFY present, emits signal after assignment.
// ----------------------------------------------------------------

// The parameter type is an unconstrained template parameter instead of
// `const type_expr &` to avoid accessing an incomplete class when type_expr
// = _nxpv2_member_type(field) (the MEMBER case).  The correct instantiation
// is pinned by the static_cast in _nxpv2_meta_setter below, which lives
// inside the registrar constructor — a complete-class context.
# define _nxpv2_gen_setter_body(prop_name, type_expr, field_ref, args)       \
    template <typename _nxpv2_setter_val_>                                   \
    void _nxpv2_add_prefix(_nxpv2_default_write_prefix, prop_name)(          \
        const _nxpv2_setter_val_ & _value)                                   \
    {                                                                        \
        field_ref = _value;                                                  \
        _nx_logic_if(_nxpv2_exists_arg(args, NOTIFY))(                       \
            NX_EMIT(_nxpv2_get_notify(prop_name, args)),                     \
            _nx_empty()                                                      \
        )                                                                    \
    }

# define _nxpv2_gen_write_dispatch(prop_name, type_expr, field_ref, args)   \
    _nx_logic_if(_nxpv2_exists_arg(args, WRITE))(                           \
        _nx_logic_if(_nxpv2_is_const(args))(                                \
            _nx_empty(),                                                     \
            _nx_logic_if(_nxpv2_has_value(args, WRITE))(                    \
                _nx_empty(),                                                 \
                _nxpv2_gen_setter_body(prop_name, type_expr, field_ref, args) \
            )                                                                \
        ),                                                                   \
        _nx_empty()                                                          \
    )

// ----------------------------------------------------------------
// Gen: reset method
//
//   bare RESET + not CONST → void reset_<name>() { ... }
//   RESET meth / CONST / no RESET → nothing
//   Resets to DEFAULT value (always present, defaults to {}).
//   If NOTIFY present, emits signal after reset.
// ----------------------------------------------------------------

# define _nxpv2_gen_reset_body(prop_name, type_expr, field_ref, args)       \
    void _nxpv2_add_prefix(_nxpv2_default_reset_prefix, prop_name)()        \
    {                                                                        \
        field_ref = _nxpv2_get_default_bare_value(args);                     \
        _nx_logic_if(_nxpv2_exists_arg(args, NOTIFY))(                      \
            NX_EMIT(_nxpv2_get_notify(prop_name, args));,                   \
            _nx_empty()                                                      \
        )                                                                    \
    }

# define _nxpv2_gen_reset_dispatch(prop_name, type_expr, field_ref, args)   \
    _nx_logic_if(_nxpv2_exists_arg(args, RESET))(                           \
        _nx_logic_if(_nxpv2_is_const(args))(                                \
            _nx_empty(),                                                     \
            _nx_logic_if(_nxpv2_has_value(args, RESET))(                    \
                _nx_empty(),                                                 \
                _nxpv2_gen_reset_body(prop_name, type_expr, field_ref, args) \
            )                                                                \
        ),                                                                   \
        _nx_empty()                                                          \
    )

// ----------------------------------------------------------------
// Meta-registration helpers
//
// Each produces the callable (pointer or lambda) passed to
// register_object_property<T>(name, ptr, getter, setter, notif, reset).
// ----------------------------------------------------------------

// getter
# define _nxpv2_meta_getter_lambda(field_ref)                               \
    [&](const _nx_self_t * _t) { return _t->field_ref; }

# define _nxpv2_meta_getter(prop_name, field_ref, has_storage, args)        \
    _nx_logic_if(_nxpv2_exists_arg(args, READ))(                            \
        _nx_logic_if(_nxpv2_has_value(args, READ))(                         \
            &_nx_self_t::_nxpv2_get_value(args, READ),                      \
            &_nx_self_t::_nxpv2_add_prefix(_nxpv2_default_read_prefix, prop_name) \
        ),                                                                   \
        _nx_logic_if(has_storage)(                                          \
            _nxpv2_meta_getter_lambda(field_ref),                           \
            nullptr                                                          \
        )                                                                    \
    )

// setter
# define _nxpv2_meta_setter_lambda(type_expr, field_ref)                    \
    [&](_nx_self_t * _t, const type_expr & _v) { _t->field_ref = _v; }

// For the bare-WRITE case the generated setter is a function template.
// We must static_cast to resolve the exact instantiation; the cast is
// evaluated inside the registrar constructor (complete-class context),
// so type_expr (possibly _nxpv2_member_type(field)) is valid there.
# define _nxpv2_meta_setter(prop_name, type_expr, field_ref, has_storage, args) \
    _nx_logic_if(_nxpv2_is_const(args))(                                    \
        nullptr,                                                             \
        _nx_logic_if(_nxpv2_exists_arg(args, WRITE))(                       \
            _nx_logic_if(_nxpv2_has_value(args, WRITE))(                    \
                &_nx_self_t::_nxpv2_get_value(args, WRITE),                 \
                static_cast<void (_nx_self_t::*)(const type_expr &)>(       \
                    &_nx_self_t::_nxpv2_add_prefix(_nxpv2_default_write_prefix, prop_name) \
                )                                                            \
            ),                                                               \
            _nx_logic_if(has_storage)(                                      \
                _nx_logic_if(_nxpv2_exists_arg(args, READ))(                \
                    nullptr,                                                 \
                    _nxpv2_meta_setter_lambda(type_expr, field_ref)         \
                ),                                                           \
                nullptr                                                      \
            )                                                                \
        )                                                                    \
    )

// notif — void(_nx_self_t*): emit signal with no arguments.
// Observers call the getter themselves if they need the current value.
# define _nxpv2_meta_notif(prop_name, args)                                 \
    _nx_logic_if(_nxpv2_exists_arg(args, NOTIFY))(                          \
        [](_nx_self_t * _t) {                                                \
            _t->_nxpv2_get_notify(prop_name, args)();                       \
        },                                                                   \
        nullptr                                                              \
    )

// reset
# define _nxpv2_meta_reset(prop_name, args)                                 \
    _nx_logic_if(_nxpv2_exists_arg(args, RESET))(                           \
        _nx_logic_if(_nxpv2_has_value(args, RESET))(                        \
            &_nx_self_t::_nxpv2_get_value(args, RESET),                     \
            &_nx_self_t::_nxpv2_add_prefix(_nxpv2_default_reset_prefix, prop_name) \
        ),                                                                   \
        nullptr                                                              \
    )

// ----------------------------------------------------------------
// Gen: meta-registration static initializer
//
//   inline static const bool _nxp_<name>_reg_ = [](){
//       _nx_self_t::static_meta_object().property_registry()
//           .register_object_property<type>(...); return true;
//   }();
// ----------------------------------------------------------------

# define _nxpv2_gen_meta_reg(prop_name, type_expr, field_ref, has_storage, args)    \
    struct _nx_concat_3(_nxp_, prop_name, _registrar_) {                    \
        _nx_concat_3(_nxp_, prop_name, _registrar_)() {                     \
            _nx_self_t::static_meta_object().property_registry()            \
                .template register_object_property<type_expr>(              \
                    #prop_name,                                              \
                    nullptr,                                                 \
                    _nxpv2_meta_getter(prop_name, field_ref, has_storage, args), \
                    _nxpv2_meta_setter(prop_name, type_expr, field_ref, has_storage, args), \
                    _nxpv2_meta_notif(prop_name, args),                     \
                    _nxpv2_meta_reset(prop_name, args)                      \
                );                                                          \
        }                                                                   \
    };                                                                      \
    inline static _nx_concat_3(_nxp_, prop_name, _registrar_)              \
        _nx_concat_3(_nxp_, prop_name, _reg_);

// ----------------------------------------------------------------
// Shared dispatcher — called by all three cases with resolved
// type_expr, field_ref and has_storage (literal 0 or 1).
// ----------------------------------------------------------------

# define _nxpv2_gen_all(prop_name, type_expr, field_ref, has_storage, args) \
    _nxpv2_gen_read_dispatch(prop_name, field_ref, args)                    \
    _nxpv2_gen_notify_dispatch(prop_name, type_expr, args)                  \
    _nxpv2_gen_write_dispatch(prop_name, type_expr, field_ref, args)        \
    _nxpv2_gen_reset_dispatch(prop_name, type_expr, field_ref, args)        \
    _nxpv2_gen_meta_reg(prop_name, type_expr, field_ref, has_storage, args)


/// ---=== ENTRY POINT ===---

// no TYPE and no MEMBER — computed property (READ must carry an explicit method name)
# define _nxpv2_generate_code_impl_0_0(prop_name, args)                           \
    _nxpv2_gen_all(prop_name,                                               \
        _nxpv2_computed_type(_nxpv2_get_value(args, READ)),                 \
        _nx_empty(),                                                         \
        0,                                                                   \
        args)

// TYPE present, no MEMBER — owned storage, generate field
# define _nxpv2_generate_code_impl_1_0(prop_name, args)                           \
    _nxpv2_gen_field_decl(prop_name, args)                                  \
    _nxpv2_gen_all(prop_name,                                               \
        _nxpv2_get_type(args),                                              \
        _nxpv2_add_prefix(_nxpv2_default_member_prefix, prop_name),         \
        1,                                                                   \
        args)

// MEMBER present, no TYPE — external storage, type inferred via decltype
# define _nxpv2_generate_code_impl_0_1(prop_name, args)                           \
    _nxpv2_gen_all(prop_name,                                               \
        _nxpv2_member_type(_nxpv2_get_existing_member(prop_name, args)),    \
        _nxpv2_get_existing_member(prop_name, args),                        \
        1,                                                                   \
        args)

// TYPE and MEMBER together — error
# define _nxpv2_generate_code_impl_1_1(prop_name, args)                           \
    static_assert(false, "NX_PROPERTY: TYPE and MEMBER are mutually exclusive")

# define _nxpv2_generate_code_impl_0(prop_name, args) \
    _nx_logic_if(_nxpv2_exists_arg(args, MEMBER)) (                         \
        _nxpv2_generate_code_impl_0_1(prop_name, args),                     \
        _nxpv2_generate_code_impl_0_0(prop_name, args)                      \
    )

# define _nxpv2_generate_code_impl_1(prop_name, args) \
    _nx_logic_if(_nxpv2_exists_arg(args, MEMBER)) (                         \
        _nxpv2_generate_code_impl_1_1(prop_name, args),                     \
        _nxpv2_generate_code_impl_1_0(prop_name, args)                      \
)

# define _nxpv2_generate_code(prop_name, args)                              \
    _nx_logic_if(_nxpv2_exists_arg(args, TYPE)) (                           \
        _nxpv2_generate_code_impl_1(prop_name, args),                       \
        _nxpv2_generate_code_impl_0(prop_name, args)                        \
    )

# define _nxpv2_entry(name, ...)                                            \
    _nxpv2_generate_code(name, _nxpv2_sort_args_d(0, __VA_ARGS__))

# ifdef NX_PROPERTY
#  undef NX_PROPERTY
# endif
# define NX_PROPERTY(name, ...) _nxpv2_entry(name, __VA_ARGS__)


# pragma endregion

#endif //NX_PROPERTY_DEFS2_HPP
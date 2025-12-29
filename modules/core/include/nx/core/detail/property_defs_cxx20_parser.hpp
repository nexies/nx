//
// Created by nexie on 29.12.2025.
//

#ifndef NXTBOT_PROPERTY_DEFS_CXX20_PARSER_HPP
#define NXTBOT_PROPERTY_DEFS_CXX20_PARSER_HPP

#include <nx/macro/args/parse.hpp>
#include "object_defs.hpp"

#define _nx_property_struct struct
#define _nx_property_type type
#define _nx_property_value value
#define _nx_property_member member
#define _nx_property_static_field static constexpr

#define TYPE                    1
#define NAME                    2
#define DESCRIPTOR_TYPE         3
#define DESCRIPTOR_NAME         4
#define READ                    5
#define WRITE                   6
#define NOTIFY                  7
#define RESET                   8
#define USING_READ              9
#define USING_WRITE             10
#define USING_NOTIFY            11
#define USING_RESET             12
#define MEMBER                  13

#define DEFAULT                 14
#define DEFAULTS                14  // alias

#define CONSTANT                15
#define CONST                   15  // alias

#define MUTABLE                 16
#define FINAL                   17

#define COMMENT                 18

#define PUBLIC                  19
#define PROTECTED               20
#define PRIVATE                 21

/**
 * Property descriptor type name. Default is property_<NAME>_t
 */
#define _nx_property_descriptor_type(...) \
    _nx_logic_if(_nx_args_contains(DESCRIPTOR_TYPE, __VA_ARGS__)) ( \
        _nx_args_get_value(DESCRIPTOR_TYPE, __VA_ARGS__), \
        _nx_apply(_nx_concat_3, property_, _nx_args_get_value(NAME, __VA_ARGS__), _t) \
    )

/**
 * Property descriptor value name. Defaults to m_property_<NAME>
 */
#define _nx_property_descriptor_name(...) \
    _nx_logic_if(_nx_args_contains(DESCRIPTOR_NAME, __VA_ARGS__)) ( \
        _nx_args_get_value(DESCRIPTOR_NAME, __VA_ARGS__), \
        _nx_apply(_nx_concat_2, m_property_, _nx_args_get_value(NAME, __VA_ARGS__)) \
    )

/**
 * Property name string
 */
#define _nx_property_name_str(...) \
    NX_TO_STRING(_nx_args_get_value(NAME, __VA_ARGS__))

/**
 * Default value if parameter present, otherwise {}
 */
#define _nx_property_default_value(...) \
    _nx_logic_if(_nx_args_contains(DEFAULT, __VA_ARGS__)) ( \
        _nx_args_get_value(DEFAULT, __VA_ARGS__), \
        {} \
    )


/**
 * Default value descriptor (if parameter is present)
 */
#define _nx_property_make_default_value_descriptor(...) \
    _nx_logic_if(_nx_args_contains(DEFAULT, __VA_ARGS__)) ( \
        _nx_property_static_field type default_value = _nx_args_get_value(DEFAULT, __VA_ARGS__);, \
        _nx_empty() \
    )



/**
 * Const modifier (if parameter is present)
 */
#define _nx_property_const_modifier(...) \
    _nx_logic_if(_nx_args_contains(CONST, __VA_ARGS__)) ( \
        const, \
        _nx_empty() \
    )

#define _nx_property_mutable_modifier(...) \
    _nx_logic_if(_nx_args_contains(MUTABLE, __VA_ARGS__)) ( \
        mutable, \
        _nx_empty() \
    )

#define _nx_property_make_value_descriptor(...) \
    _nx_logic_if(_nx_args_contains(MEMBER, __VA_ARGS__)) ( \
        _nx_empty(), \
        _nx_property_const_modifier(__VA_ARGS__) _nx_property_mutable_modifier(__VA_ARGS__) \
        _nx_property_type _nx_property_value = _nx_property_default_value(__VA_ARGS__); \
    )

#define _nx_property_make_member_descriptor(...) \
    _nx_logic_if(_nx_args_contains(MEMBER, __VA_ARGS__)) ( \
        _nx_property_static_field auto _nx_property_member = &host_type:: _nx_args_get_value(MEMBER, __VA_ARGS__);, \
        _nx_empty() \
    )

#define _nx_property_make_member_getter_descriptor(...) \
    _nx_property_static_field auto get = &host_type:: _nx_args_get_value(USING_READ, __VA_ARGS__);

// #define _nx_property_make_own_getter_descriptor(...) \
    _nx_property_static_field auto get =

#define _nx_property_make_getter_descriptor(...) \
    _nx_logic_if(_nx_args_contains(USING_READ, __VA_ARGS__)) ( \
        _nx_property_make_member_getter_descriptor(__VA_ARGS__), \


#define _nx_property_create_descriptor_struct(...) \
    _nx_property_struct _nx_property_descriptor_type(__VA_ARGS__) : ::nx::detail::property_tag<_nx_object_this_type> \
    { \
        _nx_property_static_field std::string name = _nx_property_name_str(__VA_ARGS__); \
        using _nx_property_type = _nx_args_get_value(TYPE, __VA_ARGS__); \
        _nx_property_make_default_value_descriptor(__VA_ARGS__) \
        _nx_property_make_value_descriptor(__VA_ARGS__) \
        _nx_property_make_member_descriptor(__VA_ARGS__) \
    } _nx_property_descriptor_name(__VA_ARGS__) ;



#define _nx_cxx_20_property_parser(...) \
    _nx_property_create_descriptor_struct(__VA_ARGS__)




#endif //NXTBOT_PROPERTY_DEFS_CXX20_PARSER_HPP
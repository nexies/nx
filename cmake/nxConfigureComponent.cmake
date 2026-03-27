include_guard(GLOBAL)

define_property(GLOBAL PROPERTY NX_AVAILABLE_MODULES BRIEF_DOCS "nx modules (all)"    FULL_DOCS "All nx components declared")
define_property(GLOBAL PROPERTY NX_ENABLED_MODULES   BRIEF_DOCS "nx modules (built)"  FULL_DOCS "Enabled nx components")

function(nx_configure_component _target)
    set(_one
            NAME
            VERSION
            PUBLIC_INCLUDE_DIR
            NAMESPACE
    )
    set(_multi
            PUBLIC_INCLUDES
    )

    cmake_parse_arguments(P_ "" "${_one}" "${_multi}" ${ARGN})

    set(_name ${P_NAME})
    set(_ver ${P_VERSION})
    set(_pub_inc ${P_PUBLIC_INCLUDES})
    set(_pub_inc_d ${P_PUBLIC_INCLUDE_DIR})
    set(_nmsp ${P_NAMESPACE})

    if(NOT _target)
        message(FATAL_ERROR "nx_configure_component(): TARGET is not provided")
    endif()

    if(NOT TARGET "${_target}")
        message(FATAL_ERROR "nx_configure_component(): target ${_target} does not exist")
    endif()

    if(NOT _name)
        set(_name ${_target})
    endif()

    if(NOT _ver)
        set(_ver ${NX_PROJECT_VERSION})
    endif()

    if(NOT _nmsp)
        set(_nmsp ${NX_NAMESPACE})
    endif()

    set(_alias ${_nmsp}::${_name})

    if(NOT TARGET "${_alias}")
        add_library("${_alias}" ALIAS "${_target}")
    endif()

    #TODO: nx_make_component_version( "${_name}" "${_ver}" )


endfunction()
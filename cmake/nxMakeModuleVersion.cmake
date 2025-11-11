function(nx_make_module_version)
    set(_one NAME)
    set(_multi)
    cmake_parse_arguments(_NXV "" "${_one}" "${_multi}" ${ARGN})
    if(NOT _NXV_NAME)
        message(WARNING "nx_make_module_version(): -- module NAME is not specified")
        return ()
    endif()

    set(_name ${_NXV_NAME})
    string(TOUPPER ${_name} _name_up)
    set(_MODULE_NAME ${_name})

    if(NX_${_name_up}_VERSION_STR)
        set(_MODULE_VERSION_STR ${NX_${_name_up}_VERSION_STR})
    else()
        set(_MODULE_VERSION_STR ${NX_VERSION_STR})
        set_property(GLOBAL PROPERTY NX_${_name_up}_VERSION_STR ${NX_VERSION_STR})
        set(NX_${_name_up}_VERSION_STR ${NX_VERSION_STR})
    endif()

    if(NX_${_name_up}_VERSION_MAJOR)
        set(_MODULE_VERSION_MAJOR ${NX_${_name_up}_VERSION_MAJOR})
    else()
        set(_MODULE_VERSION_MAJOR ${NX_VERSION_MAJOR})
    endif()

    if(NX_${_name_up}_VERSION_MINOR)
        set(_MODULE_VERSION_MINOR ${NX_${_name_up}_VERSION_MINOR})
    else()
        set(_MODULE_VERSION_MINOR ${NX_VERSION_MINOR})
    endif()

    if(NX_${_name_up}_VERSION_PATCH)
        set(_MODULE_VERSION_PATCH ${NX_${_name_up}_VERSION_PATCH})
    else()
        set(_MODULE_VERSION_PATCH ${NX_VERSION_PATCH})
    endif()

    configure_file(
            "${CMAKE_SOURCE_DIR}/include/nx/module_version.hpp.in"
            "${CMAKE_BINARY_DIR}/generated/nx/${_name}/version.hpp"
            @ONLY
    )

    unset(_MODULE_VERSION_PATCH)
    unset(_MODULE_VERSION_MINOR)
    unset(_MODULE_VERSION_MAJOR)
    unset(_MODULE_VERSION_STR)
    unset(_MODULE_NAME)
endfunction()
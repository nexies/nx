include_guard(GLOBAL)

function(nx_configure_demo NAME)
    set(_arg_opt LINK_PUBLIC)
    set(_arg_one COMPONENT BINARY_NAME TARGET BINARY_DIR)
    set(_arg_many SOURCE)
    cmake_parse_arguments(PARSE_ARGV 0 _arg "${_arg_opt}" "${_arg_one}" "${_arg_many}")
    # ----------------------------------------------------------------------------------------
    if(NOT NX_BUILD_EXAMPLES)
        return()
    endif()

    set(_demo_name "${NAME}")

    if(_arg_COMPONENT)
        set(_component ${_arg_COMPONENT})
        set(_demo_name "${_arg_COMPONENT}_${_demo_name}")
    endif()

    set(_demo_name "${NX_NAMESPACE}_demo_${_demo_name}")
    if(_arg_BINARY_NAME)
        set(_binary ${_arg_BINARY_NAME})
    else()
        set(_binary ${_demo_name})
    endif()

    if(_arg_TARGET)
        set(_target ${_arg_TARGET})
    else()
        set(_target ${_demo_name})
    endif()

    if(NOT _arg_SOURCE)
        set(_sources "${_demo_name}.cpp")
    else()
        set(_sources ${_arg_SOURCE})
    endif()

    if(_arg_LINK_PUBLIC)
        set(_scope PUBLIC)
    else()
        set(_scope PRIVATE)
    endif()

    if(_arg_BINARY_DIR)
        set(_binary_dir ${_arg_BINARY_DIR})
    else()
        set(_binary_dir "${NX_EXAMPLES_BINARY_DIR}")
        if(_component)
            set(_binary_dir "${_binary_dir}/${_component}")
        endif()
    endif()

    if(_component)
        set(_link_libraries "${NX_NAMESPACE}_${_component}")
    else()
        set(_link_libraries "")
    endif()

    # ----------------------------------------------------------------------------------------------
    add_executable(${_target} ${_sources})

    target_link_libraries(${_target} ${_scope} ${_link_libraries})

    if(MINGW)
        target_link_options(${_target} PRIVATE -static)
    endif()

    set_target_properties(${_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${_binary_dir}
            OUTPUT_NAME "${_binary}"
    )

    set_property(GLOBAL APPEND PROPERTY NX_ALL_DEMO_TARGETS ${_target})

endfunction()
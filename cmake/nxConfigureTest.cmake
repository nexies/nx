include_guard(GLOBAL)

function(nx_configure_test NAME)
    set(_arg_opt LINK_PRIVATE)
    set(_arg_one COMPONENT BINARY_NAME TARGET BINARY_DIR)
    set(_arg_many SOURCES)
    cmake_parse_arguments(PARSE_ARGV 0 _arg "${_arg_opt}" "${_arg_one}" "${_arg_many}")
# ----------------------------------------------------------------------------------------
    if(NOT NX_BUILD_TESTS)
        return()
    endif()

    set(_test_name "test_${NAME}")

    if(_arg_COMPONENT)
        set(_component ${_arg_COMPONENT})
        set(_test_name "${_arg_COMPONENT}_${_test_name}")
    endif()

    set(_test_name "${NX_NAMESPACE}_${_test_name}")
    if(_arg_BINARY_NAME)
        set(_binary ${_arg_BINARY_NAME})
    else()
        set(_binary ${_test_name})
    endif()

    if(_arg_TARGET)
        set(_target ${_arg_TARGET})
    else()
        set(_target ${_test_name})
    endif()

    if(NOT _arg_SOURCES)
        set(_sources "${_test_name}.cpp")
    else()
        set(_sources ${_arg_SOURCES})
    endif()

    add_executable(${_target} ${_sources})

    if(_arg_LINK_PRIVATE)
        set(_scope PRIVATE)
    else()
        set(_scope PUBLIC)
    endif()

    if(_arg_BINARY_DIR)
        set(_binary_dir ${_arg_BINARY_DIR})
    else()
        set(_binary_dir "${CMAKE_BINARY_DIR}/tests")
        if(_component)
            set(_binary_dir "${_binary_dir}/${_component}")
        endif()
    endif()

    if(_component)
        set(_link_libraries ${_scope} ${NX_NAMESPACE}_${_component})
    else()
        set(_link_libraries "")
    endif()

    target_link_libraries(${_target} ${_scope} ${_link_libraries})

    set_target_properties(${_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${_binary_dir}
            OUTPUT_NAME "${_binary}"
    )

endfunction()
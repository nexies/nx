cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED NX_MODULE_MACRO_LIMITS_MAX_NUMBER)
    set(NX_MODULE_MACRO_LIMITS_MAX_NUMBER 1024)
endif()

if(NOT DEFINED NX_MACRO_GENERATED_DIR)
    set(NX_MACRO_GENERATED_DIR "include/nx/macro/detail/generated")
endif()

set(LIMIT "${NX_MODULE_MACRO_LIMITS_MAX_NUMBER}")
set(OUT_DIR "${NX_MACRO_GENERATED_DIR}")
set(BS "\\")

function(append_text filename text)
    file(APPEND "${filename}" "${text}")
endfunction()

function(format_nx_while i filename)
    math(EXPR n "${i}+1")
    append_text("${filename}" " # define _nx_while_${i}(c, o, r, ...) _nx_while_${i}_c(c(${n}, __VA_ARGS__), c, o, r, __VA_ARGS__)\n")
    append_text("${filename}" " # define _nx_while_${i}_c(p, c, o, r, ...) _nx_logic_if(p)(_nx_while_${n}(c, o, r, o(${n}, __VA_ARGS__)), r(${n}, __VA_ARGS__))\n")
endfunction()

function(format_nx_inc i filename)
    if(i LESS 0)
        return()
    endif()

    math(EXPR n "${i}+1")
    append_text("${filename}" " # define _nx_numeric_inc_${i} ${n}\n")
endfunction()

function(format_nx_dec i filename)
    math(EXPR n "${i}-1")

    if(n LESS 0)
        append_text("${filename}" " # define _nx_numeric_dec_0 0\n")
        return()
    endif()

    append_text("${filename}" " # define _nx_numeric_dec_${i} ${n}\n")
endfunction()

function(format_nx_bool i filename)
    if(i LESS 0)
        return()
    elseif(i EQUAL 0)
        set(n 0)
    elseif(i GREATER 0)
        set(n 1)
    else()
        return()
    endif()

    append_text("${filename}" " # define _nx_logic_bool_${i} ${n}\n")
endfunction()

function(format_nx_is_max i filename)
    if(i GREATER_EQUAL LIMIT)
        set(n 1)
    else()
        set(n 0)
    endif()

    append_text("${filename}" " # define _nx_numeric_is_max_${i} ${n}\n")
endfunction()

function(format_nx_arg_tk i filename)
    append_text("${filename}" " # define _nx_arg_tk_${i} _nx_arg_tk_${i}_(\n")
    append_text("${filename}" " # define _nx_arg_tk_${i}_(...) (${i} _nx_append_va_args(__VA_ARGS__))\n")
endfunction()

function(format_nx_sequence i filename)
    math(EXPR n "${i}-1")
    set(c 1)

    if(n LESS_EQUAL 1)
        set(c 0)
    endif()

    if(i EQUAL 0)
        append_text("${filename}" " # define _nx_sequence_0(...)\n")
    else()
        append_text("${filename}" " # define _nx_sequence_${i}(c, h, m, ...) _nx_sequence_${n}(${c}, h, m, __VA_ARGS__)  h(c, _nx_apply(m, ${i}, __VA_ARGS__))\n")
    endif()
endfunction()

function(format_nx_choose_fast i filename)
    append_text("${filename}" " # define _nx_macro_impl_choose_${i}(")

    foreach(k RANGE 0 ${i})
        append_text("${filename}" "c_${k}, ")
    endforeach()

    append_text("${filename}" "...) ${BS}\n    c_${i}\n")
endfunction()

function(format_nx_choose i filename)
    math(EXPR n "${i}-1")

    if(i LESS_EQUAL 64)
        format_nx_choose_fast("${i}" "${filename}")
        return()
    endif()

    append_text("${filename}" " # define _nx_macro_impl_choose_${i}(c, ...) _nx_macro_impl_choose_${n}(__VA_ARGS__) \n")
endfunction()

function(format_nx_put_at_fast i filename)
    append_text("${filename}" " # define _nx_macro_put_at_${i}(val")

    foreach(k RANGE 0 ${i})
        append_text("${filename}" ", c_${k}")
    endforeach()

    append_text("${filename}" ", ...) ${BS}\n    ")

    if(i GREATER 0)
        math(EXPR last "${i}-1")
        foreach(k RANGE 0 ${last})
            append_text("${filename}" "c_${k}, ")
        endforeach()
    endif()

    append_text("${filename}" "val _nx_append_va_args(__VA_ARGS__)\n")
endfunction()

function(format_nx_put_at i filename)
    math(EXPR n "${i}-1")

    if(i LESS_EQUAL 64)
        format_nx_put_at_fast("${i}" "${filename}")
        return()
    endif()

    append_text("${filename}" " # define _nx_macro_put_at_${i}(val, c, ...) c, _nx_macro_put_at_${n}(val, __VA_ARGS__) \n")
endfunction()

function(generate_with_format format_name min max filename)
    foreach(i RANGE ${min} ${max})
        cmake_language(CALL "${format_name}" "${i}" "${filename}")
    endforeach()
endfunction()

function(generate_main_ifndef name limit filename)
    string(TOUPPER "${name}" upper_name)
    append_text("${filename}" "#ifndef NX_LIMITS_${upper_name}_${limit}_HPP\n")
    append_text("${filename}" "#define NX_LIMITS_${upper_name}_${limit}_HPP\n")
    append_text("${filename}" "\n")
endfunction()

function(generate_private_include name filename)
    append_text("${filename}" "// This file is auto-generated\n")
    append_text("${filename}" "// You can set NX_MODULE_MACRO_LIMITS_MAX_NUMBER variable to your desired value in CMakeFile\n\n")
    append_text("${filename}" "\n")
    append_text("${filename}" "#ifndef NX_MACRO_PRIVATE_INCLUDE\n")
    append_text("${filename}" "    #error \"This file was not meant to be included directly. Include <nx/macro/detail/${name}_limit.hpp> instead\"\n")
    append_text("${filename}" "#endif\n")
    append_text("${filename}" "\n")
endfunction()

function(generate_limit_define name limit filename)
    string(TOUPPER "${name}" upper_name)
    append_text("${filename}" "\n")
    append_text("${filename}" " # define NX_LIMITS_${upper_name} ${limit}\n")
    append_text("${filename}" "\n")
endfunction()

function(generate name limit)
    set(format_name "format_nx_${name}")
    set(filename "${OUT_DIR}/${name}_${limit}.hpp")

    file(MAKE_DIRECTORY "${OUT_DIR}")
    file(WRITE "${filename}" "")

    generate_main_ifndef("${name}" "${limit}" "${filename}")
    generate_private_include("${name}" "${filename}")
    generate_with_format("${format_name}" 0 "${limit}" "${filename}")
    generate_limit_define("${name}" "${limit}" "${filename}")
    append_text("${filename}" "#endif")
endfunction()

function(generate_powers name limit)
    set(base 128)

    if(limit LESS_EQUAL base)
        generate("${name}" "${limit}")
        return()
    endif()

    set(cur "${base}")
    while(cur LESS_EQUAL limit)
        generate("${name}" "${cur}")
        math(EXPR cur "${cur} * 2")
    endwhile()

    math(EXPR half "${cur} / 2")
    if(NOT half EQUAL limit)
        generate("${name}" "${limit}")
    endif()
endfunction()

function(is_power_of_two n out_var)
    if(n LESS_EQUAL 0)
        set(${out_var} FALSE PARENT_SCOPE)
        return()
    endif()

    math(EXPR test "${n} & (${n} - 1)")
    if(test EQUAL 0)
        set(${out_var} TRUE PARENT_SCOPE)
    else()
        set(${out_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

if(NOT LIMIT MATCHES "^[0-9]+$")
    message(FATAL_ERROR "Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER has to be an integer number, not '${LIMIT}'")
endif()

is_power_of_two("${LIMIT}" limit_is_pow2)
if(NOT limit_is_pow2)
    message(FATAL_ERROR "Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER should be a power of two (2^n), not ${LIMIT}")
endif()

if(LIMIT LESS_EQUAL 64)
    return()
endif()

generate_powers("while"    "${LIMIT}")
generate_powers("bool"     "${LIMIT}")
generate_powers("inc"      "${LIMIT}")
generate_powers("dec"      "${LIMIT}")
generate_powers("is_max"   "${LIMIT}")
generate_powers("arg_tk"   "${LIMIT}")
generate_powers("sequence" "${LIMIT}")
generate_powers("choose"   "${LIMIT}")
generate_powers("put_at"   "${LIMIT}")
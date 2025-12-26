add_library(nx_macro
        include/nx/macro.hpp
        macro.cpp
        include/nx/macro/util/overload.hpp
        include/nx/macro/detail/limits/number_inc_64.hpp
        include/nx/macro/detail/limits/number_inc_128.hpp
        include/nx/macro/detail/limits/number_inc_256.hpp
        include/nx/macro/detail/limits/number_dec_64.hpp
        include/nx/macro/detail/limits/number_dec_128.hpp
        include/nx/macro/detail/limits/number_dec_256.hpp
        include/nx/macro/detail/limits/bool_64.hpp
        include/nx/macro/detail/limits/bool_128.hpp
        include/nx/macro/detail/limits/bool_256.hpp
        include/nx/macro/detail/limits/sequence_64.hpp
        include/nx/macro/detail/number_inc_limit.hpp
        include/nx/macro/detail/number_dec_limit.hpp
        include/nx/macro/detail/sequence_limit.hpp
        include/nx/macro/detail/bool_limit.hpp
        include/nx/macro/logic/bool.hpp
        include/nx/macro/logic/if.hpp
        include/nx/macro/numeric.hpp
        include/nx/macro/logic/op.hpp
        include/nx/macro/numeric/inc_dec.hpp
        include/nx/macro/detail/limits/choose_64.hpp
        include/nx/macro/detail/choose_limit.hpp
        include/nx/macro/repeating/sequence.hpp
        include/nx/macro/repeating/repeat.hpp
        include/nx/macro/repeating/iterate.hpp
        include/nx/macro/repeating/while.hpp
        include/nx/macro/detail/limits/while_64.hpp
        include/nx/macro/detail/while_limits.hpp
        include/nx/macro/numeric/sum.hpp
        include/nx/macro/numeric/sub.hpp
        include/nx/macro/numeric/compare.hpp
        include/nx/macro/detail/limits/is_max_64.hpp
        include/nx/macro/detail/limits/is_max_128.hpp
        include/nx/macro/detail/limits/is_max_256.hpp
        include/nx/macro/detail/is_max_limit.hpp
        include/nx/macro/detail/limits/arg_tk_64.hpp
        include/nx/macro/detail/arg_tk_limit.hpp
        include/nx/macro/repeating/recursive_while.hpp
        include/nx/macro/tuple/tuple.hpp
        include/nx/macro/util/append_args.hpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_macro
        NAME macro
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)

## Generate NX_MACRO limits

# NX_MACRO_MAX_NUMBER
if(NOT NX_MODULE_MACRO_LIMITS_MAX_NUMBER)
    set (NX_MODULE_MACRO_LIMITS_MAX_NUMBER 4096)
endif()

target_compile_definitions(nx_macro PUBLIC NX_LIMITS_MAX_NUMBER=${NX_MODULE_MACRO_LIMITS_MAX_NUMBER})

# Output directory
set(NX_MODULE_MACRO_LIMITS_OUTDIR ${NX_GENERATED_INCLUDEDIR}/macro/detail/limits)

# Generator script
if(WIN32)
    set(_gen_script ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_limits.bat)
    set(_gen_cmd ${_gen_script})           # без bash
else()
    set(_gen_script ${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_limits.sh)
    set(_gen_cmd bash ${_gen_script})
endif()
set (NX_MODULE_MACRO_LIMITS_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${NX_MODULE_MACRO_LIMITS_SCRIPT})

set(NX_MODULE_MACRO_LIMITS_STAMP
        ${NX_MODULE_MACRO_LIMITS_OUTDIR}/.generated
)

add_custom_command(
        OUTPUT  ${NX_MODULE_MACRO_LIMITS_STAMP}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${NX_MODULE_MACRO_LIMITS_OUTDIR}
        COMMAND ${_gen_cmd}
        --limit ${NX_MODULE_MACRO_LIMITS_MAX_NUMBER}
        --dir   ${NX_MODULE_MACRO_LIMITS_OUTDIR}
        COMMAND ${CMAKE_COMMAND} -E touch ${NX_MODULE_MACRO_LIMITS_STAMP}
        DEPENDS ${_gen_script}
        COMMENT "Generating nx::macro_limits (limit=${NX_MODULE_MACRO_LIMITS_MAX_NUMBER})"
)

add_custom_target(nx_macro_limits
        DEPENDS ${NX_MODULE_MACRO_LIMITS_STAMP}
)

add_dependencies(nx_macro nx_macro_limits)

set(nx_macro_headers
        include/nx/macro.hpp
        macro.cpp
        include/nx/macro/util/overload.hpp
        include/nx/macro/detail/limits/inc_64.hpp
        include/nx/macro/detail/limits/dec_64.hpp
        include/nx/macro/detail/limits/bool_64.hpp
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
        include/nx/macro/detail/while_limit.hpp
        include/nx/macro/numeric/sum.hpp
        include/nx/macro/numeric/sub.hpp
        include/nx/macro/numeric/compare.hpp
        include/nx/macro/detail/limits/is_max_64.hpp
        include/nx/macro/detail/is_max_limit.hpp
        include/nx/macro/detail/limits/arg_tk_64.hpp
        include/nx/macro/detail/arg_tk_limit.hpp
        include/nx/macro/tuple/tuple.hpp
        include/nx/macro/args/count.hpp
        include/nx/macro/numeric/is_max.hpp
        include/nx/macro/detail/limits/eat_64.hpp
        include/nx/macro/detail/put_at_limit.hpp
        include/nx/macro/args/parse-2.hpp
        include/nx/macro/args/argset.hpp
        include/nx/macro/util/put_at_fast.hpp
        include/nx/macro/args/rules.hpp
)

# ---------------- Configuration -------------------

add_library(nx_macro ${nx_macro_headers})

nx_configure_component(nx_macro
        NAME macro
        VERSION 0.3.0
)

# --------------------------------------------------
# ---------------- Code generation -----------------
# --------------------------------------------------


# Max number
if(NOT NX_MODULE_MACRO_LIMITS_MAX_NUMBER)
    set (NX_MODULE_MACRO_LIMITS_MAX_NUMBER 256)
#    add_compile_options(nx_macro PUBLIC "-fbracket-depth=4096")
endif()
target_compile_definitions(nx_macro PUBLIC NX_LIMITS_MAX_NUMBER=${NX_MODULE_MACRO_LIMITS_MAX_NUMBER})

# Output directory
set(NX_MODULE_MACRO_LIMITS_OUTDIR ${NX_GENERATED_INCLUDEDIR}/macro/detail/limits)

## Generator script
set(NX_MODULE_MACRO_GENERATOR_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_limits.cmake")

## Generated stamp
set(NX_MODULE_MACRO_GENERATED_STAMP "${NX_MODULE_MACRO_LIMITS_OUTDIR}/.generated")

add_custom_command(
        OUTPUT "${NX_MODULE_MACRO_GENERATED_STAMP}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${NX_MODULE_MACRO_LIMITS_OUTDIR}"
        COMMAND ${CMAKE_COMMAND}
        -DNX_MODULE_MACRO_LIMITS_MAX_NUMBER=${NX_MODULE_MACRO_LIMITS_MAX_NUMBER}
        -DNX_MACRO_GENERATED_DIR=${NX_MODULE_MACRO_LIMITS_OUTDIR}
        -P "${NX_MODULE_MACRO_GENERATOR_SCRIPT}"
        COMMAND ${CMAKE_COMMAND} -E touch "${NX_MODULE_MACRO_LIMITS_OUTDIR}/.generated"
        DEPENDS "${NX_MODULE_MACRO_GENERATOR_SCRIPT}"
        COMMENT "Generating nx::macro_limits (limit=${NX_MODULE_MACRO_LIMITS_MAX_NUMBER})"
        VERBATIM
)

add_custom_target(nx_macro_limits
        DEPENDS ${NX_MODULE_MACRO_GENERATED_STAMP}
)

add_dependencies(nx_macro nx_macro_limits)







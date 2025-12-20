add_library(nx_macro
        include/nx/macro.hpp
        macro.cpp
        include/nx/macro/util/overload.hpp
        include/nx/macro/numeric/arithmetic_sum.hpp
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
        include/nx/macro/numeric/number_inc_dec.hpp
        include/nx/macro/repeat/iterate.hpp
        include/nx/macro/detail/limits/choose_64.hpp
        include/nx/macro/detail/choose_limit.hpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_macro
        NAME macro
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)
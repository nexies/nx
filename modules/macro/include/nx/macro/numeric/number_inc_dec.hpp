//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_NUMBER_INC_DEC_HPP
#define NX_MACRO_NUMBER_INC_DEC_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/number_dec_limit.hpp>
#include <nx/macro/detail/number_inc_limit.hpp>

#define NX_DEC(n) NX_CONCAT(_nx_dec_, n)
#define NX_INC(n) NX_CONCAT(_nx_inc_, n)

#endif //NX_MACRO_NUMBER_INC_DEC_HPP

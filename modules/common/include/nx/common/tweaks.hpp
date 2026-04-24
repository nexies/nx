//
// Created by nexie on 17.04.2026.
//

#ifndef NX_COMMON_TWEAKS_HPP
#define NX_COMMON_TWEAKS_HPP

#if !defined(NX_USE_STD_FORMAT)
#if !defined(NX_USE_EXTERNAL_FMT)
    # define NX_USE_BUNDLED_FMT
    # define FMT_HEADER_ONLY
#endif
#endif


#endif //NX_COMMON_TWEAKS_HPP

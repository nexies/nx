if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

# ── Selective component building ───────────────────────────────────────────────
# If NX_REQUIRED_COMPONENTS is defined (e.g. -DNX_REQUIRED_COMPONENTS=common;core2),
# only those subdirectories are configured.  Otherwise every module is built.
#
# The order here is the dependency order — do not reorder arbitrarily.
set(_nx_all_modules
    common
    logging
    macro
    asio
    core2
    tui
    network
    tg
)

foreach(_mod IN LISTS _nx_all_modules)
    if(NOT DEFINED NX_REQUIRED_COMPONENTS
       OR "${_mod}" IN_LIST NX_REQUIRED_COMPONENTS)
        add_subdirectory(${_mod})
    endif()
endforeach()

#    add_subdirectory(memory)
#add_subdirectory(core)

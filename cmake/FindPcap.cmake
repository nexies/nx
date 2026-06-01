# FindPcap.cmake
# ──────────────────────────────────────────────────────────────────────────────
# Finds libpcap (Linux/macOS) or Npcap/WinPcap SDK (Windows).
#
# On Windows the *runtime* installer (C:\Program Files\Npcap) does NOT include
# headers or import libraries.  You must install the Npcap SDK separately:
#   https://npcap.com/#download  →  "Npcap SDK x.x"
# Extract the ZIP anywhere and point PCAP_ROOT at the result, e.g.:
#   cmake -DPCAP_ROOT=C:/npcap-sdk ...
# or set the environment variable PCAP_ROOT / NPCAP_SDK_DIR before running cmake.
#
# Imported target
#   PCAP::PCAP
#
# Result variables
#   PCAP_FOUND          — TRUE if headers and library were found
#   PCAP_INCLUDE_DIRS   — path(s) to pcap.h / pcap/pcap.h
#   PCAP_LIBRARIES      — library to link (full path or -l flag)
#   PCAP_VERSION        — version string (when detectable via pkg-config)
#
# Hints (highest to lowest priority)
#   PCAP_ROOT           — CMake variable: root of the SDK
#   ENV PCAP_ROOT       — environment variable: same
#   ENV NPCAP_SDK_DIR   — environment variable: Npcap SDK root
#   ENV WPCAP_SDK_DIR   — environment variable: WinPcap SDK root (legacy)
# ──────────────────────────────────────────────────────────────────────────────

include(FindPackageHandleStandardArgs)

# ── 1. pkg-config (Linux / macOS only) ───────────────────────────────────────

if(NOT WIN32)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(_PCAP_PC QUIET libpcap)
    endif()
endif()

if(_PCAP_PC_FOUND)
    set(PCAP_INCLUDE_DIRS ${_PCAP_PC_INCLUDE_DIRS})
    set(PCAP_LIBRARIES    ${_PCAP_PC_LINK_LIBRARIES})
    set(PCAP_VERSION      ${_PCAP_PC_VERSION})

else()

    # ── 2. Manual search ──────────────────────────────────────────────────────

    if(WIN32)
        # Collect candidate SDK roots in priority order.
        # Note: "C:/Program Files/Npcap" is the *runtime* — it has no SDK files.
        #       The SDK must be downloaded separately and pointed to via PCAP_ROOT.

        # ── Step 1: exact hints (env / cmake vars) ────────────────────────────
        set(_pcap_roots "")
        foreach(_hint
                "${PCAP_ROOT}"
                "$ENV{PCAP_ROOT}"
                "$ENV{NPCAP_SDK_DIR}"
                "$ENV{WPCAP_SDK_DIR}"
        )
            if(NOT _hint STREQUAL "" AND IS_DIRECTORY "${_hint}")
                list(APPEND _pcap_roots "${_hint}")
            endif()
        endforeach()

        # ── Step 2: glob versioned dirs (e.g. npcap-sdk-1.6, npcap-sdk-1.13) ─
        # For each parent directory, expand patterns like "npcap-sdk*", "WpdPack*".
        set(_pcap_glob_parents
            "C:/"
            "C:/devel"
            "C:/dev"
            "C:/tools"
            "C:/sdk"
            "C:/libs"
            "$ENV{ProgramFiles}"
            "$ENV{ProgramW6432}"
            "$ENV{USERPROFILE}"
            "$ENV{USERPROFILE}/Downloads"
        )
        set(_pcap_glob_patterns
            "npcap-sdk*"
            "Npcap-sdk*"
            "npcap_sdk*"
            "Npcap_sdk*"
            "npcap*sdk*"
            "WpdPack*"
            "WinPcap*"
            "libpcap*"
        )
        foreach(_parent ${_pcap_glob_parents})
            if(IS_DIRECTORY "${_parent}")
                foreach(_pat ${_pcap_glob_patterns})
                    file(GLOB _pcap_glob_hits LIST_DIRECTORIES true "${_parent}/${_pat}")
                    foreach(_hit ${_pcap_glob_hits})
                        if(IS_DIRECTORY "${_hit}")
                            list(APPEND _pcap_roots "${_hit}")
                        endif()
                    endforeach()
                endforeach()
            endif()
        endforeach()

        # ── Step 3: well-known exact fallbacks ────────────────────────────────
        foreach(_hint
                "C:/npcap-sdk"
                "C:/Npcap-sdk"
                "C:/npcap_sdk"
                "C:/WpdPack"
                "C:/WinPcap"
        )
            list(APPEND _pcap_roots "${_hint}")
        endforeach()

        list(REMOVE_DUPLICATES _pcap_roots)

        # Npcap and WinPcap both ship wpcap.lib (not pcap.lib)
        set(_pcap_lib_names wpcap pcap)

        # Architecture-aware library subfolder
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_pcap_lib_subdirs Lib/x64 Lib)
        else()
            set(_pcap_lib_subdirs Lib/x86 Lib)
        endif()

    elseif(APPLE)
        set(_pcap_roots "")
        foreach(_hint
                "${PCAP_ROOT}"
                "$ENV{PCAP_ROOT}"
                "/opt/homebrew"     # Apple Silicon Homebrew
                "/usr/local"        # Intel Homebrew / classic
                "/opt/local"        # MacPorts
                "/usr"
        )
            if(NOT _hint STREQUAL "")
                list(APPEND _pcap_roots "${_hint}")
            endif()
        endforeach()

        set(_pcap_lib_names pcap)
        set(_pcap_lib_subdirs lib)

    else()
        # Linux / other POSIX
        set(_pcap_roots "")
        foreach(_hint
                "${PCAP_ROOT}"
                "$ENV{PCAP_ROOT}"
                "/usr/local"
                "/usr"
        )
            if(NOT _hint STREQUAL "")
                list(APPEND _pcap_roots "${_hint}")
            endif()
        endforeach()

        set(_pcap_lib_names pcap)
        set(_pcap_lib_subdirs
            lib
            lib64
            lib/x86_64-linux-gnu
            lib/aarch64-linux-gnu
            lib/arm-linux-gnueabihf
        )
    endif()

    # ── Header ────────────────────────────────────────────────────────────────

    find_path(PCAP_INCLUDE_DIR
        NAMES pcap/pcap.h pcap.h
        HINTS ${_pcap_roots}
        PATH_SUFFIXES include Include
        NO_DEFAULT_PATH
    )
    # Second pass: let CMake search system-default paths as well
    if(NOT PCAP_INCLUDE_DIR)
        find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h pcap.h)
    endif()

    # ── Library ───────────────────────────────────────────────────────────────

    find_library(PCAP_LIBRARY
        NAMES ${_pcap_lib_names}
        HINTS ${_pcap_roots}
        PATH_SUFFIXES ${_pcap_lib_subdirs}
        NO_DEFAULT_PATH
    )
    if(NOT PCAP_LIBRARY)
        find_library(PCAP_LIBRARY NAMES ${_pcap_lib_names})
    endif()

    set(PCAP_INCLUDE_DIRS ${PCAP_INCLUDE_DIR})
    set(PCAP_LIBRARIES    ${PCAP_LIBRARY})

endif() # pkg-config branch

# ── 3. Validate ───────────────────────────────────────────────────────────────

find_package_handle_standard_args(Pcap
    REQUIRED_VARS PCAP_LIBRARIES PCAP_INCLUDE_DIRS
    VERSION_VAR   PCAP_VERSION
)

mark_as_advanced(PCAP_INCLUDE_DIR PCAP_LIBRARY PCAP_INCLUDE_DIRS PCAP_LIBRARIES)

# ── 4. Diagnostics when not found on Windows ──────────────────────────────────

if(NOT PCAP_FOUND AND WIN32)
    message(STATUS
        "FindPcap: SDK not found.  The Npcap *runtime* installer does not "
        "include headers or import libraries.\n"
        "  1. Download the Npcap SDK from https://npcap.com/#download\n"
        "  2. Extract the ZIP, e.g. to C:\\npcap-sdk\n"
        "  3. Re-run cmake with -DPCAP_ROOT=C:/npcap-sdk\n"
        "     or set the environment variable PCAP_ROOT before running cmake."
    )
endif()

# ── 5. Imported target ────────────────────────────────────────────────────────

if(PCAP_FOUND AND NOT TARGET PCAP::PCAP)
    add_library(PCAP::PCAP UNKNOWN IMPORTED)

    set_target_properties(PCAP::PCAP PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PCAP_INCLUDE_DIRS}"
    )

    if(PCAP_LIBRARIES)
        list(GET PCAP_LIBRARIES 0 _pcap_first_lib)
        if(EXISTS "${_pcap_first_lib}")
            set_target_properties(PCAP::PCAP PROPERTIES
                IMPORTED_LOCATION "${_pcap_first_lib}"
            )
        else()
            # pkg-config returned bare flags like "-lpcap"
            set_target_properties(PCAP::PCAP PROPERTIES
                IMPORTED_LOCATION        ""
                INTERFACE_LINK_LIBRARIES "${PCAP_LIBRARIES}"
            )
        endif()
    endif()

    # On Windows the DLL (wpcap.dll) lives in C:\Windows\System32\Npcap\
    # and is loaded at runtime via PATH — no extra action needed here.
endif()

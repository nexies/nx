add_library(nx_core
        src/core.cpp
        include/nx/core/Context.hpp
        include/nx/core/Object.hpp
        include/nx/core/macro/logic.hpp
        include/nx/core/macro/argname.hpp
        include/nx/core/Connection.hpp
        #        include/nx/core/Callable.hpp
#        include/nx/core/Tuple.hpp
)

#target_sources(nx_core PUBLIC
#    FILE_SET CXX_MODULES FILES
#        src/reflect.cppm
#)

include(nxMakeModule)
nx_make_module(
        TARGET nx_core
        NAME core
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)


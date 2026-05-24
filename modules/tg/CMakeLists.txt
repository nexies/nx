find_package(nlohmann_json CONFIG REQUIRED)

add_library(nx_tg
    src/bot.cpp
)

target_link_libraries(nx_tg PUBLIC
    nx::core2
    nx::network
    nlohmann_json::nlohmann_json
)

nx_configure_component(nx_tg
    NAME tg
    VERSION 0.1.0
)

add_subdirectory(demo)

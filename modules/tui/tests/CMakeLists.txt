
include(nxConfigureTest)

nx_configure_test(escape_parser
        COMPONENT tui
        SOURCE test_escape_parser.cpp
)

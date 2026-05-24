include(nxConfigureDemo)

nx_configure_demo(echo_bot
    COMPONENT tg
    SOURCE echo_bot_demo.cpp
    LINK_PUBLIC nx_logging
)

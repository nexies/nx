add_executable(nx_core_test_asio_context
        asio_context.cpp
        handle_notifier.cpp
)
target_link_libraries(nx_core_test_asio_context PUBLIC nx::core)

add_executable(nx_core_test_handle_notifier
        handle_notifier.cpp
)
target_link_libraries(nx_core_test_handle_notifier PUBLIC nx::core)

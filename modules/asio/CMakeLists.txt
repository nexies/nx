set(nx_asio_includes
        include/nx/asio/handle_notifier.hpp
        include/nx/asio/io_context.hpp
)

set(nx_asio_sources
        src/backend/backend.cpp
        src/backend/backend_epoll.inl
        src/backend/backend_iocp.inl
        src/backend/backend_kevent.inl

        src/context/io_context.cpp
        src/context/context_impl.cpp
        src/context/context_impl.hpp

        src/handle_notifier/handle_notifier.cpp
        src/handle_notifier/handle_notifier_impl.cpp
        src/handle_notifier/handle_notifier_impl.hpp

        src/signal_set/signal_set.cpp
        src/signal_set/signal_set_impl.cpp
        src/signal_set/signal_set_impl.cpp

        src/steady_timer.cpp
        src/context/reactor_handle.cpp
#        tests/test_steady_timer.cpp
)


add_library (nx_asio
        ${nx_asio_headers}
        ${nx_asio_sources}
)

target_link_libraries(nx_asio PUBLIC
        nx::common
)

nx_configure_component(nx_asio
        NAME asio
        VERSION 0.0.3
)

add_subdirectory(tests)
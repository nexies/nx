add_library (nx_asio
        src/backend/backend.cpp
        src/backend/backend_epoll.inl
        src/backend/backend_iocp.inl
        src/backend/backend_kevent.inl

        src/context/io_context.cpp
        src/context/context_impl.cpp
        src/context/context_impl.hpp

        include/nx/asio/handle_notifier.hpp
        src/handle_notifier/handle_notifier.cpp
        src/handle_notifier/handle_notifier_impl.cpp
        src/handle_notifier/handle_notifier_impl.hpp

        src/signal_set/signal_set.cpp
        src/signal_set/signal_set_impl.cpp
        src/signal_set/signal_set_impl.cpp

        src/steady_timer.cpp
        src/context/reactor_handle.cpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_asio
        NAME asio
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        VERSION 0.0.2
)

target_link_libraries(nx_asio PUBLIC
        nx::common
)
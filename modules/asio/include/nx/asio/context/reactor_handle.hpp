//
// Created by nexie on 23.03.2026.
//

#ifndef NX_REACTOR_HANDLE_HPP
#define NX_REACTOR_HANDLE_HPP

#include <nx/asio/backend/backend_types.hpp>
#include <nx/asio/context/io_context.hpp>

#include <functional>
#include "nx/common/helpers.hpp"

namespace nx::asio
{
    // reactor_handle — строительный блок для любого I/O объекта.
    //
    // Два способа использования:
    //
    //   1. Композиция (предпочтительно для новых типов):
    //        class tcp_socket {
    //            reactor_handle reactor_;
    //            ...
    //            reactor_.set_read_handler([this](backend_event & ev) { ... });
    //        };
    //
    //   2. Наследование (для типов с нестандартной диспетчеризацией):
    //        class signal_set_posix : public reactor_handle {
    //            void on_event(backend_event & ev) override { ... }
    //        };
    //
    // Handlers срабатывают one-shot: после вызова очищаются.
    // Чтобы продолжить получать события — установи handler снова внутри callback.

    class reactor_handle
    {
        friend class io_context::impl;
    public:
        using handler_t = std::function<void(backend_event &)>;

        explicit reactor_handle(io_context & ctx);
        virtual ~reactor_handle();

        NX_DISABLE_COPY(reactor_handle)

        // Регистрация в event loop
        void
        install(native_handle_t handle, io_interest interest);

        void
        modify(native_handle_t handle, io_interest interest);

        void
        uninstall();

        // Установка обработчиков (one-shot: срабатывают один раз, потом очищаются)
        void
        set_read_handler (handler_t h);

        void
        set_write_handler(handler_t h);

        NX_NODISCARD io_interest
        interest()  const;

        NX_NODISCARD native_handle_t
        handle()    const;

        NX_NODISCARD bool
        installed() const;

        NX_NODISCARD io_context &
        ctx()       const;

    protected:
        // Виртуальный — subclass'ы могут переопределить для нестандартной логики.
        // Реализация по умолчанию диспетчеризует read/write/error к stored handlers.
        virtual void
        on_event(backend_event & event);

    private:
        io_context &     ctx_;
        io_interest      interest_ { io_interest::none };
        native_handle_t  handle_   { g_null_handle };
        bool             installed_ { false };
        handler_t        read_handler_;
        handler_t        write_handler_;
    };

}

#endif //NX_REACTOR_HANDLE_HPP

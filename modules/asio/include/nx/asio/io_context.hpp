//
// Created by nexie on 29.03.2026.
//

#ifndef NX_ASIO_IO_CONTEXT_HPP
#define NX_ASIO_IO_CONTEXT_HPP

#include <nx/asio/context/io_context.hpp>
#include <nx/asio/context/reactor_handle.hpp>

namespace nx::asio {

    inline void
    post (io_context & ctx, io_context::task_t && task) {
        ctx.post(std::move(task));
    }

    inline void
    dispatch (io_context & ctx, io_context::task_t && task) {
        ctx.dispatch(std::move(task));
    }

}

#endif //NX_ASIO_IO_CONTEXT_HPP

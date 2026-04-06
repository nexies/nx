//
// Created by nexie on 31.03.2026.
//

#include <nx/common/types/errors/error_descriptor.hpp>

namespace {
    constexpr size_t g_pool_size = 512;

    using data_t = nx::error_descriptor;
    using ptr_t = nx::error_descriptor *;

    std::mutex g_mutex;
    size_t g_current_pos;
    data_t g_pool [g_pool_size];
    ptr_t  g_ptrs [g_pool_size];

    std::atomic_bool g_is_pool_init { false };

    inline void
    try_pool_init() {
        if (g_is_pool_init)
            return;

        std::lock_guard lg (g_mutex);

        for (auto i = 0; i < g_pool_size; ++i) {
            g_ptrs[i] = g_pool + i;
        }
        g_current_pos = 0;
        g_is_pool_init = true;
    }

    ptr_t pool_allocate () {
        try_pool_init();
        std::lock_guard lg(g_mutex);

        if (g_current_pos >= g_pool_size)
            return nullptr;

        return g_ptrs[g_current_pos++];
    }

    void pool_free (ptr_t p) {
        std::lock_guard lg (g_mutex);

        if (g_current_pos == 0)
            return;

        g_ptrs[g_current_pos--] = p;
    }

}

namespace nx {
    error_descriptor::error_descriptor(const nx::source_location &location, const std::string &comment)
        : location { location }
        , comment { comment }
    {
    }

    error_descriptor::error_descriptor(const std::string &comment)
        : location { g_undefined_location }
        , comment { comment }
    {

    }

    error_descriptor::error_descriptor(const error_descriptor &other) = default;

    error_descriptor::error_descriptor()
        : location { g_undefined_location }
        , comment {}
    {
    }

    error_descriptor *
    make_error_descriptor(const nx::source_location &location, const std::string &comment) {
        auto ptr = pool_allocate();
        if (!ptr)
            return nullptr;

        return new (ptr) error_descriptor(location, comment);
    }

    error_descriptor *
    make_error_descriptor(const error_descriptor &other) {
        auto ptr = pool_allocate();
        if (!ptr)
            return nullptr;

        return new (ptr) error_descriptor(other);
    }

    void free_error_descriptor(error_descriptor * ptr) {
        ptr->what_cache.clear();
        ptr->comment.clear();
        ptr->location = nx::g_undefined_location;

        pool_free(ptr);
    }
}

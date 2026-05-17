#pragma once

#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>

#include <atomic>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace nx::core { class thread; }

namespace nx::network {

// ── resolver ──────────────────────────────────────────────────────────────────
//
// Hostname → endpoint list resolver backed by getaddrinfo.
//
// Async: call resolve(); listen to resolved / error_occurred signals.
//   The resolver must be assigned to a thread (move_to_thread) before use.
//   Requests are queued on an internal worker thread so the caller's thread
//   is never blocked.
//
// Sync: call resolve_sync() from any thread; blocks until getaddrinfo returns.

class resolver : public nx::core::object {
public:
    NX_OBJECT(resolver)
    NX_DISABLE_COPY(resolver)
    NX_DISABLE_MOVE(resolver)

    explicit resolver(nx::core::object * parent = nullptr);
    ~resolver() override;

    // Async resolve — posts to an internal worker thread; emits resolved or
    // error_occurred on the resolver's assigned thread when done.
    nx::result<void> resolve(std::string_view             host,
                              uint16_t                     port,
                              std::optional<socket_family> hint = std::nullopt);

    // Blocking resolve — usable from any thread, no thread assignment needed.
    NX_NODISCARD static nx::result<std::vector<endpoint>>
    resolve_sync(std::string_view             host,
                 uint16_t                     port,
                 std::optional<socket_family> hint = std::nullopt) noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(resolved,       std::string, std::vector<endpoint>)
    NX_SIGNAL(error_occurred, nx::error)

private:
    std::shared_ptr<std::atomic_bool> alive_;
    std::unique_ptr<nx::core::thread> worker_;
};

} // namespace nx::network

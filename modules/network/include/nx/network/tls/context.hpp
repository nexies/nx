#pragma once

#include <nx/common/types/result.hpp>

#include <openssl/ssl.h>

#include <memory>
#include <string>

namespace nx::network::tls {

class context {
public:
    enum class role { client, server };

    explicit context(role r = role::client);

    // Server: load PEM certificate / private key from files
    nx::result<void> use_certificate_file(const std::string & path);
    nx::result<void> use_private_key_file(const std::string & path);

    // Server: generate an ephemeral RSA-2048 self-signed cert for localhost/127.0.0.1
    nx::result<void> use_ephemeral_self_signed();

    // Client: load CA certificates from the OS trust store
    // (Windows Certificate Store on Win32; default paths on Linux/macOS).
    // Called automatically for role::client in the constructor.
    nx::result<void> load_system_cas();

    // Client: load extra CA bundle from a PEM file
    nx::result<void> load_verify_locations(const std::string & cafile);

    // Toggle peer verification (default on for client, off for server)
    void set_verify_peer(bool v) noexcept;

    SSL_CTX * native_handle() const noexcept;
    role      get_role()      const noexcept;

private:
    role role_;
    std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)> ctx_;
};

} // namespace nx::network::tls

#include <nx/network/tls/context.hpp>
#include <nx/common/platform.hpp>
#include <nx/common/types/result.hpp>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#if defined(NX_OS_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <wincrypt.h>
// wincrypt.h defines these as integer constants that collide with OpenSSL types.
#  undef X509_NAME
#  undef X509_CERT_PAIR
#  undef X509_EXTENSIONS
#  undef PKCS7_SIGNER_INFO
#endif

#include <stdexcept>
#include <string>

namespace nx::network::tls {

namespace {

nx::error ssl_err(std::string_view msg)
{
    char buf[256] = {};
    ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
    return nx::err::runtime_error(std::string(msg) + ": " + buf);
}

} // namespace

context::context(role r)
    : role_(r)
    , ctx_(nullptr, &SSL_CTX_free)
{
    SSL_CTX * raw = SSL_CTX_new(TLS_method());
    if (!raw)
        throw std::runtime_error("SSL_CTX_new failed");
    ctx_.reset(raw);

    if (r == role::client) {
        SSL_CTX_set_verify(raw, SSL_VERIFY_PEER, nullptr);
        load_system_cas(); // ignore error — context may still work with load_verify_locations
    } else {
        SSL_CTX_set_verify(raw, SSL_VERIFY_NONE, nullptr);
    }
}

nx::result<void> context::load_system_cas()
{
#if defined(NX_OS_WINDOWS)
    // OpenSSL on Windows has no default CA path — load from the Windows trust store.
    X509_STORE * store = SSL_CTX_get_cert_store(ctx_.get());

    for (const wchar_t * store_name : { L"ROOT", L"CA" }) {
        HCERTSTORE hStore = CertOpenSystemStoreW(0, store_name);
        if (!hStore) continue;

        PCCERT_CONTEXT pCert = nullptr;
        while ((pCert = CertEnumCertificatesInStore(hStore, pCert)) != nullptr) {
            const unsigned char * p = pCert->pbCertEncoded;
            X509 * x509 = d2i_X509(nullptr, &p,
                                    static_cast<long>(pCert->cbCertEncoded));
            if (x509) {
                X509_STORE_add_cert(store, x509);
                X509_free(x509);
            }
        }
        CertCloseStore(hStore, 0);
    }
    return {};
#else
    if (SSL_CTX_set_default_verify_paths(ctx_.get()) != 1)
        return ssl_err("load_system_cas");
    return {};
#endif
}

nx::result<void> context::use_certificate_file(const std::string & path)
{
    if (SSL_CTX_use_certificate_file(ctx_.get(), path.c_str(), SSL_FILETYPE_PEM) != 1)
        return ssl_err("use_certificate_file");
    return {};
}

nx::result<void> context::use_private_key_file(const std::string & path)
{
    if (SSL_CTX_use_PrivateKey_file(ctx_.get(), path.c_str(), SSL_FILETYPE_PEM) != 1)
        return ssl_err("use_private_key_file");
    return {};
}

nx::result<void> context::load_verify_locations(const std::string & cafile)
{
    if (SSL_CTX_load_verify_locations(ctx_.get(), cafile.c_str(), nullptr) != 1)
        return ssl_err("load_verify_locations");
    return {};
}

nx::result<void> context::use_ephemeral_self_signed()
{
    // Generate RSA-2048 key
    EVP_PKEY_CTX * kctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!kctx) return ssl_err("EVP_PKEY_CTX_new_id");

    if (EVP_PKEY_keygen_init(kctx) <= 0) {
        EVP_PKEY_CTX_free(kctx);
        return ssl_err("EVP_PKEY_keygen_init");
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(kctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(kctx);
        return ssl_err("EVP_PKEY_CTX_set_rsa_keygen_bits");
    }
    EVP_PKEY * pkey = nullptr;
    if (EVP_PKEY_keygen(kctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(kctx);
        return ssl_err("EVP_PKEY_keygen");
    }
    EVP_PKEY_CTX_free(kctx);

    // Build X.509 v3 certificate
    X509 * cert = X509_new();
    if (!cert) { EVP_PKEY_free(pkey); return ssl_err("X509_new"); }

    X509_set_version(cert, 2);                                       // v3
    ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
    X509_gmtime_adj(X509_getm_notBefore(cert), 0);
    X509_gmtime_adj(X509_getm_notAfter(cert), 365L * 24 * 3600);

    X509_NAME * name = X509_get_subject_name(cert);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
        reinterpret_cast<const unsigned char *>("localhost"), -1, -1, 0);
    X509_set_issuer_name(cert, name);
    X509_set_pubkey(cert, pkey);

    // SAN: DNS:localhost + IP:127.0.0.1
    {
        X509V3_CTX v3ctx;
        X509V3_set_ctx_nodb(&v3ctx);
        X509V3_set_ctx(&v3ctx, cert, cert, nullptr, nullptr, 0);
        if (X509_EXTENSION * ext = X509V3_EXT_conf_nid(
                nullptr, &v3ctx, NID_subject_alt_name,
                "DNS:localhost,IP:127.0.0.1")) {
            X509_add_ext(cert, ext, -1);
            X509_EXTENSION_free(ext);
        }
    }

    X509_sign(cert, pkey, EVP_sha256());

    const bool ok = SSL_CTX_use_certificate(ctx_.get(), cert) == 1
                 && SSL_CTX_use_PrivateKey(ctx_.get(), pkey)  == 1
                 && SSL_CTX_check_private_key(ctx_.get())     == 1;

    X509_free(cert);
    EVP_PKEY_free(pkey);

    if (!ok) return ssl_err("use_ephemeral_self_signed");
    return {};
}

void context::set_verify_peer(bool v) noexcept
{
    SSL_CTX_set_verify(ctx_.get(), v ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, nullptr);
}

SSL_CTX * context::native_handle() const noexcept { return ctx_.get(); }
context::role context::get_role()  const noexcept { return role_; }

} // namespace nx::network::tls

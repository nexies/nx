// https_server_demo — HTTPS/1.1 demo using tls::stream + http::router.
//
// Run without arguments — an ephemeral self-signed cert is generated in memory:
//   https_server_demo
//
// Or supply your own PEM files:
//   https_server_demo cert.pem key.pem
//
// Open https://127.0.0.1:8443 in the browser.
// The self-signed cert will trigger a browser warning — click "Proceed anyway".
//
// Routes:
//   GET  /               → HTML demo page
//   GET  /api/greet/:name → JSON greeting
//   POST /api/echo       → echoes body as plain text
//   GET  /api/stop       → graceful shutdown

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <cstdlib>
#include <string>

// ── Inline demo page ──────────────────────────────────────────────────────────

static constexpr const char * DEMO_HTML = R"html(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>nx::network HTTPS demo</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', system-ui, sans-serif;
      background: #0f170f;
      color: #e2f0e2;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 3rem 1rem;
    }
    h1 { font-size: 2rem; font-weight: 700; color: #6ee7b7; margin-bottom: 0.4rem; }
    .subtitle { color: #6b7280; font-size: 0.9rem; margin-bottom: 2.5rem; }
    .card {
      background: #1a2e1a; border: 1px solid #2d4e2d; border-radius: 12px;
      padding: 1.5rem; width: 100%; max-width: 540px; margin-bottom: 1.25rem;
    }
    .card h2 { font-size: 0.85rem; text-transform: uppercase; letter-spacing: 0.08em; color: #7ca87c; margin-bottom: 1rem; }
    .row { display: flex; gap: 0.5rem; }
    input, textarea {
      flex: 1; background: #0f170f; border: 1px solid #2d4e2d; border-radius: 8px;
      color: #e2f0e2; font-family: inherit; font-size: 0.9rem;
      padding: 0.55rem 0.85rem; outline: none; transition: border-color 0.15s;
    }
    input:focus, textarea:focus { border-color: #059669; }
    textarea { resize: vertical; min-height: 80px; }
    button {
      background: #059669; color: #fff; border: none; border-radius: 8px;
      padding: 0.55rem 1.1rem; font-size: 0.9rem; cursor: pointer;
      transition: background 0.15s; white-space: nowrap;
    }
    button:hover { background: #047857; }
    .result {
      margin-top: 0.85rem; background: #0f170f; border: 1px solid #2d4e2d;
      border-radius: 8px; padding: 0.65rem 0.85rem;
      font-family: 'Cascadia Code', 'Fira Code', monospace;
      font-size: 0.82rem; color: #6ee7b7; min-height: 2.5rem; word-break: break-all;
    }
    .badge {
      display: inline-block; font-size: 0.7rem; font-weight: 700;
      letter-spacing: 0.05em; padding: 0.15rem 0.45rem;
      border-radius: 4px; margin-right: 0.4rem; vertical-align: middle;
    }
    .get  { background: #064e3b; color: #6ee7b7; }
    .post { background: #3b1f06; color: #fbbf24; }
    .lock { font-size: 0.8rem; color: #6ee7b7; margin-bottom: 1.5rem; }
  </style>
</head>
<body>
  <h1>nx::network</h1>
  <p class="lock">&#128274; HTTPS/1.1 — TLS via OpenSSL</p>
  <p class="subtitle">C++ async server with tls::stream</p>

  <div class="card">
    <h2><span class="badge get">GET</span> /api/greet/:name</h2>
    <div class="row">
      <input id="greet-name" type="text" placeholder="your name" value="world">
      <button onclick="greet()">Send</button>
    </div>
    <div class="result" id="greet-out">—</div>
  </div>

  <div class="card">
    <h2><span class="badge post">POST</span> /api/echo</h2>
    <textarea id="echo-body" placeholder="type something to echo...">Hello over TLS!</textarea>
    <div class="row" style="margin-top:0.5rem">
      <button onclick="echo()">Send</button>
    </div>
    <div class="result" id="echo-out">—</div>
  </div>

  <script>
    async function greet() {
      const name = document.getElementById('greet-name').value || 'world';
      const out  = document.getElementById('greet-out');
      try {
        const r = await fetch('/api/greet/' + encodeURIComponent(name));
        out.textContent = await r.text();
      } catch(e) { out.textContent = 'Error: ' + e.message; }
    }
    async function echo() {
      const body = document.getElementById('echo-body').value;
      const out  = document.getElementById('echo-out');
      try {
        const r = await fetch('/api/echo', {
          method: 'POST', headers: { 'Content-Type': 'text/plain' }, body
        });
        out.textContent = await r.text();
      } catch(e) { out.textContent = 'Error: ' + e.message; }
    }
  </script>
</body>
</html>
)html";

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    nx::logging::set_default_log_domain("https_demo");

    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    // ── TLS context ───────────────────────────────────────────────────────────

    tls::context ctx(tls::context::role::server);

    if (argc >= 3) {
        // Use supplied cert/key files
        if (auto r = ctx.use_certificate_file(argv[1]); !r) {
            nx_critical("Failed to load certificate '{}': {}", argv[1], r.error().what());
            return 1;
        }
        if (auto r = ctx.use_private_key_file(argv[2]); !r) {
            nx_critical("Failed to load private key '{}': {}", argv[2], r.error().what());
            return 1;
        }
        nx_info("TLS: loaded certificate from '{}'", argv[1]);
    } else {
        // Generate an ephemeral self-signed cert (demo mode)
        if (auto r = ctx.use_ephemeral_self_signed(); !r) {
            nx_critical("Failed to generate ephemeral cert: {}", r.error().what());
            return 1;
        }
        nx_info("TLS: using ephemeral self-signed certificate for localhost");
    }

    // ── Router ────────────────────────────────────────────────────────────────

    http::router router;

    router.get("/", [](const http::request &, auto respond) {
        http::response res;
        res.status_code = 200;
        res.reason      = "OK";
        res.headers["Content-Type"] = "text/html; charset=utf-8";
        res.body = DEMO_HTML;
        respond(std::move(res));
    });

    router.get("/api/greet/:name", [](const http::request & req, auto respond) {
        const auto name = req.param("name");
        const std::string greeting = name.empty() ? "Hello, stranger!"
                                                   : "Hello, " + std::string(name) + "!";
        respond(http::response::ok(greeting + "\n", "application/json"));
    });

    router.post("/api/echo", [](const http::request & req, auto respond) {
        respond(http::response::ok(std::string(req.body_sv())));
    });

    router.get("/api/stop", [&loop](const http::request &, auto respond) {
        respond(http::response::ok("Shutting down...\n"));
        loop.exit(0);
    });

    // ── HTTPS server ──────────────────────────────────────────────────────────

    using tls_stream   = tls::stream<tcp::socket>;
    using https_server = http::server<tls_stream>;

    https_server srv(
        [&ctx](std::shared_ptr<tcp::socket> raw) {
            return std::make_shared<tls_stream>(std::move(raw), ctx);
        });

    nx::core::connect(&srv, &https_server::request_received, &srv,
        [&router](http::request req, https_server::responder_t respond) {
            nx_info("{} {}", req.method, req.target);
            router.handle(std::move(req), std::move(respond));
        });

    nx::core::connect(&srv, &https_server::error_occurred, &srv,
        [](nx::error e) { nx_critical("server error: {}", e.what()); });

    srv.listen(endpoint { ip_address::loopback_v4(), 8443 })
        .or_else([](nx::error e) {
            nx_critical("listen failed: {}", e.what());
            std::exit(1);
        });

    nx_info("HTTPS server listening on https://127.0.0.1:8443");

    return loop.exec();
}

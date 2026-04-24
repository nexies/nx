//
// core2 — master include for the nx::core2 module.
//
// Brings in the complete public API:
//   nx::core::object, thread, local_thread, loop, timer, io_device, application
//   NX_OBJECT, NX_SIGNAL, NX_EMIT, NX_PROPERTY macros
//   connect(), disconnect(), emit() free functions
//

#pragma once

// ── Object system ─────────────────────────────────────────────────────────────
#include <nx/core2/object/object.hpp>

// ── Thread / event loop ───────────────────────────────────────────────────────
#include <nx/core2/thread/thread.hpp>
#include <nx/core2/loop/loop.hpp>

// ── Timer ─────────────────────────────────────────────────────────────────────
#include <nx/core2/timer/timer.hpp>

// ── I/O ───────────────────────────────────────────────────────────────────────
#include <nx/core2/io_device.hpp>

// ── Application ───────────────────────────────────────────────────────────────
#include <nx/core2/app/application.hpp>

#pragma once

namespace nx::tui {

struct key_event;
struct mouse_event;

// ── event_filter ──────────────────────────────────────────────────────────────
//
// Override filter_key() or filter_mouse() to intercept events before they
// reach the target widget (or the default application handling).
//
// Return true  to consume the event — no further handling occurs.
// Return false to let the event pass through to the next filter / handler.
//
// Installation:
//   tui_application::install_event_filter(filter)  — app-wide, before all routing.
//   widget::install_event_filter(filter)            — per-widget, before on_key_press / on_mouse_press.
//
// Ownership: the application / widget does NOT own the filter.
// The caller must ensure the filter outlives its installation.

class event_filter {
public:
    virtual ~event_filter() = default;

    virtual bool filter_key  (key_event   & e) { (void)e; return false; }
    virtual bool filter_mouse(mouse_event & e) { (void)e; return false; }
};

} // namespace nx::tui

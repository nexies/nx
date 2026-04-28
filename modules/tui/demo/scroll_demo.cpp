// scroll_demo — demonstrates scroll_area with vertical and 2D content.
//
// Widget tree:
//
//   screen
//   └── root (v_box)
//       ├── title_lbl       label  fixed h=1
//       ├── panels_row      h_box  spacing=1
//       │   ├── left_frame  frame(rounded, "Vertical — 30 items")
//       │   │   └── v_sa    scroll_area  [tab-focusable]
//       │   │       └── v_box — 30 colored labels (fixed h=1 each)
//       │   └── right_frame frame(rounded, "2D — 120 cols × 40 rows")
//       │       └── td_sa   scroll_area  [tab-focusable]
//       │           └── v_box — 40 labels (fixed h=1, text 120 chars)
//       └── status_lbl      label  fixed h=1  ← updated on scroll
//
// Tab             — cycle focus between scroll areas
// Arrow keys      — scroll 1 cell
// Page Up/Down    — scroll one page
// Home / End      — jump to start / end
// Mouse wheel     — scroll vertically
// Escape          — quit

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/scroll_area.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/box.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>

#include <string>

using namespace nx::tui;

// ── scroll_controller ─────────────────────────────────────────────────────────

class scroll_controller : public nx::core::object {
    label       * status_;
    scroll_area * v_sa_;
    scroll_area * td_sa_;
    screen      * scr_;

public:
    NX_OBJECT(scroll_controller)

    scroll_controller(label * status, scroll_area * v_sa, scroll_area * td_sa,
                      screen * scr, nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , status_(status), v_sa_(v_sa), td_sa_(td_sa), scr_(scr)
    {}

    void update_status()
    {
        const std::string txt =
            "  [Vertical] row=" + std::to_string(v_sa_->scroll_y()) +
            "   [2D] col=" + std::to_string(td_sa_->scroll_x()) +
            "  row=" + std::to_string(td_sa_->scroll_y()) +
            "     Tab: next  Arrows/PgUp/PgDn/Home/End: scroll  Esc: quit";
        status_->set_text(txt);
        scr_->render();
    }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto * scr = app.main_screen();
    scr->set_style(h_gradient_bg(color::rgb(10, 10, 30), color::rgb(30, 10, 20)));

    // ── Widget tree ───────────────────────────────────────────────────────────

    auto * root = new v_box(scr);

    // Title bar.
    auto * title_lbl = new label(root);
    title_lbl->set_fixed_height(1);
    title_lbl->set_text("  nx::tui  scroll_area demo");
    title_lbl->set_style(fg(color::white)
        | h_gradient_bg(color::rgb(40, 40, 80), color::rgb(80, 20, 60)));

    // ── Panels row ────────────────────────────────────────────────────────────

    auto * panels_row = new h_box(root);
    panels_row->set_spacing(1);

    // ── Left panel: vertical scroll (30 items) ────────────────────────────────

    auto * left_frame = new frame(panels_row);
    left_frame->set_border_style(border_style::rounded);
    left_frame->set_title("Vertical — 30 items");
    left_frame->set_border_color(color::cyan);

    auto * v_sa = new scroll_area(left_frame);

    auto * items_box = new v_box(v_sa);

    for (int i = 0; i < 30; ++i) {
        auto * row = new label(items_box);
        row->set_fixed_height(1);
        row->set_text("  Item " + std::to_string(i + 1)
                      + "  — use wheel or arrow keys to scroll");

        const float t = static_cast<float>(i) / 29.0f;
        row->set_style(fg(color::interpolate(t,
            color::rgb(20, 200, 220), color::rgb(220, 60, 120))));
    }

    // ── Right panel: 2D scroll (40 rows × 120 cols) ───────────────────────────

    auto * right_frame = new frame(panels_row);
    right_frame->set_border_style(border_style::rounded);
    right_frame->set_title("2D \xe2\x80\x94 120 cols \xc3\x97 40 rows");
    right_frame->set_border_color(color::magenta);

    auto * td_sa = new scroll_area(right_frame);
    td_sa->set_content_width(120);
    td_sa->set_content_height(40);

    auto * td_box = new v_box(td_sa);

    for (int i = 0; i < 40; ++i) {
        auto * row = new label(td_box);
        row->set_fixed_height(1);

        // Build a 120-char line: "Row NN | ABCDEF..."
        const std::string prefix = "Row " + std::to_string(i + 1) + " | ";
        std::string line = prefix;
        line.reserve(120);
        for (int c = static_cast<int>(line.size()); c < 120; ++c)
            line += static_cast<char>('A' + (c + i) % 26);

        row->set_text(line);
        row->set_style(fg(color::interpolate(
            static_cast<float>(i) / 39.0f,
            color::rgb(100, 220, 100),
            color::rgb(220, 100, 200)
        )));
    }

    // ── Status bar ────────────────────────────────────────────────────────────

    auto * status_lbl = new label(root);
    status_lbl->set_fixed_height(1);
    status_lbl->set_style(fg(color::gray_light));

    // ── Signals ───────────────────────────────────────────────────────────────

    scroll_controller ctrl(status_lbl, v_sa, td_sa, scr);

    nx::core::connect(v_sa,  &scroll_area::scrolled,
                      &ctrl, &scroll_controller::update_status);
    nx::core::connect(td_sa, &scroll_area::scrolled,
                      &ctrl, &scroll_controller::update_status);

    ctrl.update_status();
    scr->set_focused_widget(v_sa);

    return app.exec();
}

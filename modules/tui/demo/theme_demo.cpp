// theme_demo — demonstrates the nx::tui theme system.
//
// Shows all major widget types rendered with the active theme.
// Press Tab / Shift+Tab to cycle focus.
// Press 'D' to switch to the dark theme.
// Press 'L' to switch to the light theme.
// Press Escape to quit.
//
// Widget tree:
//   screen
//   └── root (v_box, spacing=0)
//       ├── header_frame   frame  (title = "nx::tui theme demo")
//       │   └── header_lbl label
//       ├── main (h_box, spacing=1)
//       │   ├── left (v_box, spacing=1, fixed w=30)
//       │   │   ├── section: Controls
//       │   │   │   ├── btn_normal  button  "Normal button"
//       │   │   │   ├── btn_accent  button  "Accent button"   (highlight color)
//       │   │   │   └── btn_status  button  "Status: dark"
//       │   │   └── section: Input
//       │   │       ├── edit_lbl    label   "Enter text:"
//       │   │       └── edit        line_edit
//       │   └── right (v_box, spacing=1)
//       │       ├── colors_frame  frame  (title = "Color roles")
//       │       │   └── colors_box v_box  — one label per role
//       │       └── scroll_frame  frame  (title = "Scrollable list")
//           └── scroll_area → v_box of labels

#include <fmt/format.h>

#include <nx/tui/application.hpp>
#include <nx/tui/input/event_filter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/widgets/scroll_area.hpp>
#include <nx/tui/layout/box.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/types/theme.hpp>

using namespace nx::tui;

// ── Helpers ───────────────────────────────────────────────────────────────────

static label * make_label(const std::string & text, widget * parent,
                          style_option style = {})
{
    auto * l = new label(parent);
    l->set_text(text);
    if (!style.empty()) l->set_style(style);
    return l;
}

// A small colored swatch label showing a theme role name.
class role_swatch : public widget
{
    theme_role role_;
    std::string name_;

public:
    NX_OBJECT(role_swatch)

    role_swatch(theme_role r, std::string name, widget * parent = nullptr)
        : widget(parent), role_(r), name_(std::move(name))
    {
        set_fixed_height(1);
    }

protected:
    void on_paint(painter & p) override
    {
        const color bg_c = p.theme_bg   (role_);
        const color fg_c = p.theme_color(role_);

        // Draw bg swatch (2 cells) + name
        if (bg_c != color::default_color) {
            p.apply_style(bg(bg_c) | fg(fg_c != color::default_color
                                         ? fg_c
                                         : p.theme_color(theme_role::foreground)));
            p.draw_char({0, 0}, "  ");
        } else {
            p.apply_style(fg(fg_c != color::default_color
                              ? fg_c
                              : p.theme_color(theme_role::foreground)));
            p.draw_char({0, 0}, "fg");
        }

        p.apply_style(fg(p.theme_color(theme_role::foreground))
                    | bg(p.theme_bg(theme_role::background)));
        p.draw_text({3, 0}, name_);
    }
};

// ── App-level event filter (global key handler) ───────────────────────────────

class global_keys : public event_filter
{
    button * status_btn_;

public:
    explicit global_keys(button * status_btn)
        : status_btn_(status_btn) {}

    bool filter_key(key_event & e) override
    {
        if (e.code != key::printable) return false;

        if (e.character == U'd' || e.character == U'D') {
            nx_tui_app->set_theme(theme::dark());
            status_btn_->set_text("Status: dark");
            nx_tui_app->main_screen()->update();
            return true;
        }
        if (e.character == U'l' || e.character == U'L') {
            nx_tui_app->set_theme(theme::light());
            status_btn_->set_text("Status: light");
            nx_tui_app->main_screen()->update();
            return true;
        }
        if (e.character == U'h' || e.character == U'H')
        {
            nx_tui_app->set_theme(theme::halloween());
            status_btn_->set_text("Status: halloween");
            nx_tui_app->main_screen()->update();
            return true;
        }
        return false;
    }
};

// ── Main ──────────────────────────────────────────────────────────────────────

int main()
{
    tui_application app;
    app.set_theme(theme::dark());

    auto * scr  = app.main_screen();
    auto * root = new v_box(scr);
    root->set_spacing(0);

    // ── Header ────────────────────────────────────────────────────────────────

    auto * hdr = new frame(root);
    hdr->set_border_style(border_style::single);
    hdr->set_title("nx::tui theme demo");
    hdr->set_fixed_height(3);
    make_label("Press D = dark theme   L = light theme  H = halloween  Tab = cycle focus   Esc = quit",
               hdr, fg(color::default_color));

    // ── Main row ──────────────────────────────────────────────────────────────

    auto * main_row = new h_box(root);
    main_row->set_spacing(1);

    // ── Left column ───────────────────────────────────────────────────────────

    auto * left = new v_box(main_row);
    left->set_spacing(1);
    left->set_fixed_width(32);
    left->set_horizontal_policy(size_policy::fixed);

    // Controls frame
    auto * ctrl_frame = new frame(left);
    ctrl_frame->set_title("Controls");
    ctrl_frame->set_border_style(border_style::rounded);

    auto * ctrl_box = new v_box(ctrl_frame);
    ctrl_box->set_spacing(1);

    auto * btn_normal = new button(ctrl_box);
    btn_normal->set_text("Normal button");

    auto * btn_accent = new button(ctrl_box);
    btn_accent->set_text("Accent button");
    // Set explicit accent color via the override API.
    // (In a real app you might leave this to the theme too.)

    auto * btn_status = new button(ctrl_box);
    btn_status->set_text("Status: dark");

    // Input frame
    auto * input_frame = new frame(left);
    input_frame->set_title("Input");
    input_frame->set_border_style(border_style::rounded);

    auto * input_box = new v_box(input_frame);
    input_box->set_spacing(0);

    make_label("Enter text:", input_box);
    auto * edit = new line_edit(input_box);
    (void)edit;

    // ── Right column ──────────────────────────────────────────────────────────

    auto * right = new v_box(main_row);
    right->set_spacing(1);

    // Color roles frame
    auto * roles_frame = new frame(right);
    roles_frame->set_title("Color roles");
    roles_frame->set_border_style(border_style::rounded);
    roles_frame->set_fixed_height(35);

    auto * roles_box = new v_box(roles_frame);
    roles_box->set_spacing(0);

    const struct { theme_role r; const char * name; } roles[] = {
        { theme_role::background,          "background"          },
        { theme_role::background_alt,      "background_alt"      },
        { theme_role::foreground,          "foreground"          },
        { theme_role::foreground_dim,      "foreground_dim"      },
        { theme_role::foreground_disabled, "foreground_disabled" },
        { theme_role::primary,             "primary"             },
        { theme_role::primary_content,     "primary_content"     },
        { theme_role::secondary,           "secondary"           },
        { theme_role::secondary_content,   "secondary_content"   },
        { theme_role::accent,              "accent"              },
        { theme_role::accent_content,      "accent_content"      },
        { theme_role::neutral,             "neutral"             },
        { theme_role::neutral_content,     "neutral_content"     },
        { theme_role::control,             "control"             },
        { theme_role::control_hover,       "control_hover"       },
        { theme_role::control_active,      "control_active"      },
        { theme_role::control_disabled,    "control_disabled"    },
        { theme_role::selection,           "selection"           },
        { theme_role::selection_text,      "selection_text"      },
        { theme_role::border,              "border"              },
        { theme_role::border_focus,        "border_focus"        },
        { theme_role::scrollbar,           "scrollbar"           },
        { theme_role::scrollbar_thumb,     "scrollbar_thumb"     },
        { theme_role::info,                "info"                },
        { theme_role::info_content,        "info_content"        },
        { theme_role::success,             "success"             },
        { theme_role::success_content,     "success_content"     },
        { theme_role::warning,             "warning"             },
        { theme_role::warning_content,     "warning_content"     },
        { theme_role::error,               "error"               },
        { theme_role::error_content,       "error_content"       },
    };

    for (const auto & [r, name] : roles)
        new role_swatch(r, name, roles_box);

    // Scrollable list frame
    auto * scroll_frame = new frame(right);
    scroll_frame->set_title("Scrollable list");
    scroll_frame->set_border_style(border_style::rounded);

    auto * sa = new scroll_area(scroll_frame);
    sa->set_focus_policy(widget::focus_policy::tab_focus);

    auto * sa_content = new v_box(sa);
    sa_content->set_spacing(0);
    for (int i = 1; i <= 30; ++i)
        make_label(fmt::format("  List item {:2d}", i), sa_content);

    // ── Event filter ──────────────────────────────────────────────────────────

    auto gk = global_keys(btn_status);
    app.install_event_filter(&gk);

    // ── Signals ───────────────────────────────────────────────────────────────

    nx::core::connect(btn_normal, &button::clicked, &app, [&]() {
        edit->set_text("Normal clicked");
    });

    nx::core::connect(btn_accent, &button::clicked, &app, [&]() {
        edit->set_text("Accent clicked");
    });

    return app.exec();
}

// animation_demo — demonstrates the animation system.
//
// Shows:
//   animated_button  — smooth hover/focus color transitions via animated_value<color>
//   progress_bar     — animated_value<float> driven by a repeating timer
//   fade_label       — text that fades in/out on key press
//
// Tab/Shift+Tab    — cycle focus
// Space / Enter    — trigger progress bar animation on focused button
// F               — start fade animation on the label
// Escape           — quit

#include <nx/tui/application.hpp>
#include <nx/tui/animation/animation.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <../include/nx/tui/layout/box.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>

#include <fmt/format.h>
#include <string>
#include <algorithm>

using namespace nx::tui;

// ── color palette ────────────────────────────────────────────────────────────

static color k_bg         = color::rgb( 18,  18,  24);
static color k_btn_normal = color::rgb( 50,  50,  70);
static color k_btn_hover  = color::rgb( 80,  80, 130);
static color k_btn_focus  = color::rgb( 60, 120, 200);
static color k_text       = color::rgb(220, 220, 240);
static color k_bar_empty  = color::rgb( 40,  40,  55);
static color k_bar_full   = color::rgb( 60, 180, 100);
static color k_accent     = color::rgb(255, 180,  60);

// ── animated_button ───────────────────────────────────────────────────────────
//
// Button with smooth color transitions:
//   - hover:  k_btn_normal → k_btn_hover    (150ms ease_out)
//   - focus:  adds k_btn_focus tint         (200ms ease_out)
//   - click:  brief flash to k_accent       (80ms ease_out, then back)

class animated_button : public widget {
    std::string label_;

    animated_value<color> bg_   { this, k_btn_normal };
    animated_value<color> text_ { this, k_text        };
    animator *            flash_ = new animator(this);

    bool flashing_ = false;

public:
    NX_OBJECT(animated_button)

    explicit    animated_button(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::tab_focus);
        set_fixed_height(3);
    }

    void set_label(std::string t) { label_ = std::move(t); update(); }

    NX_SIGNAL(clicked)

protected:
    void on_paint(painter & p) override
    {
        p.apply_style(bg(bg_.value()));
        p.fill(" ");

        // Border
        const int w = size().width;
        const int h = size().height;
        if (w >= 2 && h >= 2) {
            p.apply_style(fg(color::interpolate(
                flash_->value(), bg_.value(), k_accent)));
            for (int col = 0; col < w; ++col) {
                p.draw_char({col,     0    }, "─");
                p.draw_char({col,     h - 1}, "─");
            }
            for (int row = 1; row < h - 1; ++row) {
                p.draw_char({0,     row}, "│");
                p.draw_char({w - 1, row}, "│");
            }
            p.draw_char({0,     0    }, "╭"); p.draw_char({w - 1, 0    }, "╮");
            p.draw_char({0,     h - 1}, "╰"); p.draw_char({w - 1, h - 1}, "╯");
        }

        // Label centered on middle row
        if (h >= 1 && !label_.empty()) {
            const int row = h / 2;
            const int x   = std::max(1, (w - static_cast<int>(label_.size())) / 2);
            p.apply_style(fg(text_.value()) | bg(bg_.value()));
            if (has_focus()) p.enable_style(pixel_style_flag::bold);
            p.draw_text({x, row}, label_);
            if (has_focus()) p.disable_style(pixel_style_flag::bold);
        }
    }

    void on_focus_in(  ) override
    {
        widget::on_focus_in();
        bg_.animate_to(k_btn_focus, 200);
    }
    void on_focus_out( ) override
    {
        widget::on_focus_out();
        bg_.animate_to(is_hovered() ? k_btn_hover : k_btn_normal, 200);
    }
    void on_mouse_enter(mouse_event & e) override
    {
        widget::on_mouse_enter(e);
        if (!has_focus()) bg_.animate_to(k_btn_hover, 150);
    }
    void on_mouse_leave(mouse_event & e) override
    {
        widget::on_mouse_leave(e);
        if (!has_focus()) bg_.animate_to(k_btn_normal, 150);
    }

    void on_mouse_press(mouse_event & e) override
    {
        if (e.button == mouse_button::left) { _do_click(); e.accept(); }
    }
    void on_key_press(key_event & e) override
    {
        if (e.code == key::enter ||
            (e.code == key::printable && e.character == U' '))
        {
            _do_click();
            e.accept();
        }
    }

private:
    void _do_click()
    {
        // Flash border to accent, then fade back.
        flash_->animate_to(1.0f, 80, easing::ease_out, false);
        nx::core::connect(flash_, &animator::finished, this,
                          [this]() {
                              flash_->animate_to(0.0f, 200, easing::ease_in, false);
                          },
                          nx::core::connection_type::auto_t,
                          nx::core::connection_flag::single_shot);
        NX_EMIT(clicked)
    }
};

// ── progress_bar ──────────────────────────────────────────────────────────────

class progress_bar : public widget {
    animated_value<float> fill_ { this, 0.0f };

public:
    NX_OBJECT(progress_bar)

    explicit progress_bar(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::no_focus);
        set_fixed_height(1);
    }

    void animate_to(float target, int ms = 600, easing_fn fn = easing::ease_in_out)
    {
        fill_.animate_to(target, ms, fn);
    }

    void set_progress(float v) { fill_.set(v); update(); }

    [[nodiscard]] float     progress() const noexcept { return fill_.value(); }
    [[nodiscard]] animator & raw()     noexcept       { return fill_.raw();   }

protected:
    void on_paint(painter & p) override
    {
        p.fill(" ");
        const int w     = size().width;
        const int filled = static_cast<int>(static_cast<float>(w) * fill_.value());
        for (int col = 0; col < w; ++col) {
            const color c = col < filled ? k_bar_full : k_bar_empty;
            p.apply_style(bg(c) | fg(c));
            p.draw_char({col, 0}, col < filled ? "█" : "░");
        }
    }
};

// ── fade_label ────────────────────────────────────────────────────────────────

class fade_label : public widget {
    std::string           text_;
    animated_value<color> color_ { this, color::rgb(0, 0, 0) };

public:
    NX_OBJECT(fade_label)

    explicit fade_label(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_focus_policy(focus_policy::no_focus);
        set_fixed_height(1);
    }

    void set_text(std::string t) { text_ = std::move(t); }

    void fade_in(color target = k_accent, int ms = 400)
    {
        color_.animate_to(target, ms, easing::ease_out);
    }
    void fade_out(int ms = 300)
    {
        color_.animate_to(k_bg, ms, easing::ease_in);
    }

protected:
    void on_paint(painter & p) override
    {
        p.fill(" ");
        p.apply_style(fg(color_.value()));
        p.draw_text({0, 0}, text_);
    }
};

// ── demo controller ───────────────────────────────────────────────────────────

class demo_controller : public nx::core::object {
    progress_bar * bar_;
    fade_label   * msg_;
    label        * pct_lbl_;
    float          target_   = 0.0f;
    int            step_     = 0;

public:
    NX_OBJECT(demo_controller)

    demo_controller(progress_bar * bar, fade_label * msg, label * pct,
                    nx::core::object * parent = nullptr)
        : nx::core::object(parent), bar_(bar), msg_(msg), pct_lbl_(pct)
    {}

    void on_btn1() { _advance(0.25f, "  +25%"); }
    void on_btn2() { _advance(0.50f, "  +50%"); }
    void on_btn3() { _set(0.0f,      "  Reset"); }

    void on_tick()
    {
        pct_lbl_->set_text(fmt::format("  {:3.0f}%", bar_->progress() * 100.0f));
    }

private:
    void _advance(float delta, const char * label_text)
    {
        target_ = std::clamp(target_ + delta, 0.0f, 1.0f);
        bar_->animate_to(target_);
        msg_->set_text(label_text);
        msg_->fade_in();
        nx::core::connect(&bar_->raw(), &animator::finished, this,
                          [this]() { msg_->fade_out(); },
                          nx::core::connection_type::auto_t,
                          nx::core::connection_flag::single_shot);
    }

    void _set(float v, const char * label_text)
    {
        target_ = v;
        bar_->animate_to(v, 400, easing::ease_in_out);
        msg_->set_text(label_text);
        msg_->fade_in();
        nx::core::connect(&bar_->raw(), &animator::finished, this,
                          [this]() { msg_->fade_out(); },
                          nx::core::connection_type::auto_t,
                          nx::core::connection_flag::single_shot);
    }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto * scr = app.main_screen();
    scr->set_style(bg(k_bg) | fg(k_text));

    auto * root = new v_box(scr);
    root->set_spacing(1);

    // Title
    auto * title = new label(root);
    title->set_text("  nx::tui  Animation Demo  —  Tab: focus   Escape: quit");
    title->set_style(fg(k_accent));
    title->set_fixed_height(1);

    // Button row
    auto * btn_row = new h_box(root);
    btn_row->set_fixed_height(3);
    btn_row->set_spacing(2);

    auto * btn1 = new animated_button(btn_row);
    btn1->set_label("+25%");
    btn1->set_fixed_width(12);

    auto * btn2 = new animated_button(btn_row);
    btn2->set_label("+50%");
    btn2->set_fixed_width(12);

    auto * btn3 = new animated_button(btn_row);
    btn3->set_label("Reset");
    btn3->set_fixed_width(12);

    new widget(btn_row); // spacer

    // Progress bar row
    auto * bar_row = new h_box(root);
    bar_row->set_fixed_height(1);
    bar_row->set_spacing(1);

    auto * pct_lbl = new label(bar_row);
    pct_lbl->set_text("    0%");
    pct_lbl->set_fixed_width(7);
    pct_lbl->set_style(fg(k_text));

    auto * bar = new progress_bar(bar_row);

    // Fade message
    auto * msg = new fade_label(root);
    msg->set_text("");
    msg->set_fixed_height(1);

    // Wire up
    scr->set_focused_widget(btn1);

    auto ctrl = new demo_controller (bar, msg, pct_lbl, &app);

    nx::core::connect(btn1, &animated_button::clicked,
                      ctrl, &demo_controller::on_btn1);
    nx::core::connect(btn2, &animated_button::clicked,
                      ctrl, &demo_controller::on_btn2);
    nx::core::connect(btn3, &animated_button::clicked,
                      ctrl, &demo_controller::on_btn3);

    // Update percentage label every animation tick.
    app.animations().add_on_tick([&ctrl]() { ctrl->on_tick(); });

    return app.exec();
}

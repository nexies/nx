// layout_demo — demonstrates every nx::tui layout container.
//
// Widget tree:
//   screen
//   └── root (v_box)
//       ├── title_bar  (label, h=1)
//       ├── tab_bar    (h_stack, h=3, align=start, spacing=1)
//       │   └── button × 7   (one per layout type)
//       ├── pages      (tab_layout, expanding)
//       │   ├── 0  v_box / h_box
//       │   ├── 1  v_stack / h_stack
//       │   ├── 2  a_box  (overlay layers)
//       │   ├── 3  flow_layout
//       │   ├── 4  grid_layout
//       │   ├── 5  tab_layout (nested)
//       │   └── 6  free_layout
//       └── status_bar (label, h=1)
//
// Navigation:  ← → (arrows) or 1–7 to jump directly.
// Ctrl+C: quit.

#include <nx/tui/application.hpp>
#include <nx/tui/widgets/screen.hpp>
#include <nx/tui/widgets/label.hpp>
#include <nx/tui/widgets/button.hpp>
#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/layout/box.hpp>
#include <nx/tui/layout/stack.hpp>
#include <nx/tui/layout/flow.hpp>
#include <nx/tui/layout/grid.hpp>
#include <nx/tui/layout/tab_layout.hpp>
#include <nx/tui/layout/free_layout.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/style_modifier.hpp>
#include <nx/tui/input/event_filter.hpp>

#include <array>
#include <string>
#include <vector>

using namespace nx::tui;

// ── colored_box ───────────────────────────────────────────────────────────────
// Simple widget: fills with its style background, draws centered label text.

class colored_box : public widget {
    std::string text_;
public:
    NX_OBJECT(colored_box)

    colored_box(const std::string & text, color bg_c,
                nx::core::object * parent = nullptr)
        : widget(parent), text_(text)
    {
        set_style(bg(bg_c) | fg(color::white));
        set_focus_policy(focus_policy::no_focus);
    }

protected:
    void on_paint(painter & p) override
    {
        if (text_.empty()) return;
        const int cx = std::max(0, (size().width  - static_cast<int>(text_.size())) / 2);
        const int cy = std::max(0, (size().height - 1) / 2);
        p.draw_text({cx, cy}, text_);
    }
};

// ── badge_widget ──────────────────────────────────────────────────────────────
// Third overlay layer: paints a small badge without clearing the layers below.
// No background is set at widget level, so the renderer skips the background
// fill — only the specific badge cells are written.

class badge_widget : public widget {
public:
    NX_OBJECT(badge_widget)

    explicit badge_widget(nx::core::object * parent = nullptr)
        : widget(parent)
    {
        set_transparent(true);
        set_focus_policy(focus_policy::no_focus);
    }

protected:
    void on_paint(painter & p) override
    {
        p.apply_style(fg(color::black) | bg(color::yellow));
        p.draw_text({1, 1}, " Layer 3: badge ");
    }
};

// ── tab_controller ────────────────────────────────────────────────────────────
// Connects tab buttons ↔ tab_layout, keeps button styles in sync.

class tab_controller : public nx::core::object {
    std::vector<button *> btns_;
    tab_layout          * pages_;

    void _refresh_styles() {
        const int cur = pages_->current();
        for (int i = 0; i < static_cast<int>(btns_.size()); ++i) {
            if (i == cur)
                btns_[i]->set_style(fg(color::black) | bg(color::cyan));
            else
                btns_[i]->set_style(fg(color::cyan)  | bg(color::rgb(20,20,40)));
        }
    }

public:
    NX_OBJECT(tab_controller)

    tab_controller(std::vector<button *> btns, tab_layout * pages,
                   nx::core::object * parent = nullptr)
        : nx::core::object(parent), btns_(std::move(btns)), pages_(pages)
    {
        for (int i = 0; i < static_cast<int>(btns_.size()); ++i) {
            nx::core::connect(btns_[i], &button::clicked,
                              this,     [this, i]() { go(i); });
        }
        nx::core::connect(pages_, &tab_layout::current_changed,
                          this,   [this](int) { _refresh_styles(); });
        _refresh_styles();
    }

    void go(int idx)        { pages_->set_current(idx); }
    void next()             { go((pages_->current() + 1) % pages_->count()); }
    void prev()             { go((pages_->current() - 1 + pages_->count()) % pages_->count()); }
    tab_layout * pages()    { return pages_; }
};

// ── app_key_filter ────────────────────────────────────────────────────────────
// ← → arrows and 1–7 digit keys switch pages globally.

class app_key_filter : public event_filter {
    tab_controller * ctrl_;
public:
    explicit app_key_filter(tab_controller * c) : ctrl_(c) {}

    bool filter_key(key_event & e) override {
        if (e.code == key::arrow_left)  { ctrl_->prev(); return true; }
        if (e.code == key::arrow_right) { ctrl_->next(); return true; }
        if (e.code == key::printable &&
            e.character >= char32_t('1') && e.character <= char32_t('7')) {
            ctrl_->go(static_cast<int>(e.character - char32_t('1')));
            return true;
        }
        return false;
    }
    bool filter_mouse(mouse_event &) override { return false; }
};

// ── page helpers ──────────────────────────────────────────────────────────────

static color palette(int i) {
    const color cols[] = {
        color::rgb(180, 60,  60),
        color::rgb(60,  160, 60),
        color::rgb(60,  100, 200),
        color::rgb(180, 140, 40),
        color::rgb(140, 60,  180),
        color::rgb(40,  160, 160),
        color::rgb(200, 100, 40),
    };
    return cols[i % 7];
}

// ── pages ─────────────────────────────────────────────────────────────────────

// Page 0: v_box and h_box
static widget * make_page_boxes(nx::core::object * parent)
{
    auto * page = new h_box(parent);
    page->set_spacing(1);
    page->set_margin(1);

    // ── left: v_box ──────────────────────────────────────────────────────────
    auto * lf = new frame(page);
    lf->set_title("v_box");
    lf->set_border_color(color::cyan);

    auto * vb = new v_box(lf);
    vb->set_spacing(1);
    vb->set_margin(1);

    auto * b1 = new colored_box("fixed h=2", palette(0), vb);
    b1->set_fixed_height(2);

    auto * b2 = new colored_box("expanding (stretch=1)", palette(1), vb);
    (void)b2;

    auto * b3 = new colored_box("fixed h=3", palette(2), vb);
    b3->set_fixed_height(3);

    auto * b4 = new colored_box("expanding (stretch=2)", palette(3), vb);
    b4->set_stretch_factor(2);

    // ── right: h_box ─────────────────────────────────────────────────────────
    auto * rf = new frame(page);
    rf->set_title("h_box");
    rf->set_border_color(color::magenta);

    auto * hb = new h_box(rf);
    hb->set_spacing(1);
    hb->set_margin(1);

    auto * h1 = new colored_box("fixed w=10", palette(4), hb);
    h1->set_fixed_width(10);

    auto * h2 = new colored_box("expanding", palette(5), hb);
    (void)h2;

    auto * h3 = new colored_box("fixed w=10", palette(6), hb);
    h3->set_fixed_width(10);

    return page;
}

// Page 1: v_stack and h_stack with all three alignment modes
static widget * make_page_stacks(nx::core::object * parent)
{
    auto * page = new h_box(parent);
    page->set_spacing(1);
    page->set_margin(1);

    const stack_alignment aligns[] = {
        stack_alignment::start,
        stack_alignment::center,
        stack_alignment::end,
    };
    const char * align_names[] = { "v_stack:start", "v_stack:center", "v_stack:end" };

    for (int a = 0; a < 3; ++a) {
        auto * fr = new frame(page);
        fr->set_title(align_names[a]);
        fr->set_border_color(palette(a));

        auto * st = new v_stack(fr);
        st->set_spacing(1);
        st->set_margin(1);
        st->set_alignment(aligns[a]);

        for (int i = 0; i < 3; ++i) {
            auto * b = new colored_box("item " + std::to_string(i + 1), palette(i + a), st);
            b->set_fixed_height(3);
        }
    }

    const char * h_names[] = { "h_stack:start", "h_stack:center", "h_stack:end" };

    for (int a = 0; a < 3; ++a) {
        auto * fr = new frame(page);
        fr->set_title(h_names[a]);
        fr->set_border_color(palette(a + 3));

        auto * st = new h_stack(fr);
        st->set_spacing(1);
        st->set_margin(1);
        st->set_alignment(aligns[a]);

        for (int i = 0; i < 3; ++i) {
            auto * b = new colored_box(std::to_string(i + 1), palette(i + a + 3), st);
            b->set_fixed_width(8);
        }
    }

    return page;
}

// Page 2: a_box — layered rendering
static widget * make_page_abox(nx::core::object * parent)
{
    auto * page = new v_box(parent);
    page->set_margin(1);

    // Explanation label at the top.
    auto * desc = new label(page);
    desc->set_fixed_height(2);
    desc->set_text("  a_box: all children share the same rect and paint in order.\n"
                   "  Transparent layers skip the pre-paint clear so lower layers show through.");
    desc->set_style(fg(color::gray_light));

    // The a_box itself fills the rest.
    auto * fr = new frame(page);
    fr->set_title("a_box  (3 layers)");
    fr->set_border_color(color::yellow);

    auto * ab = new a_box(fr);

    // Layer 1: solid background fill (opaque, default transparent=false).
    auto * bg_layer = new colored_box("Layer 1 — background", color::rgb(20, 40, 80), ab);
    (void)bg_layer;

    // Layer 2: overlay frame — transparent=true so the renderer skips clear()
    // and the frame's own inner fill is also skipped.  Only the border cells
    // are written on top of layer 1.
    auto * overlay = new frame(ab);
    overlay->set_title("Layer 2 — overlay frame");
    overlay->set_border_color(color::cyan);
    overlay->set_border_style(border_style::double_);
    overlay->set_transparent(true);

    // Layer 3: corner badge — transparent=true, draws only its text cells.
    auto * badge = new badge_widget(ab);
    (void)badge;
    (void)overlay;

    return page;
}

// Page 3: flow_layout — wrapping tag cloud
static widget * make_page_flow(nx::core::object * parent)
{
    auto * page = new v_box(parent);
    page->set_margin(1);

    auto * desc = new label(page);
    desc->set_fixed_height(1);
    desc->set_text("  flow_layout: items placed left-to-right, wrapping to the next row "
                   "when the line is full.  Resize the terminal to see wrap behaviour.");
    desc->set_style(fg(color::gray_light));

    auto * fr = new frame(page);
    fr->set_title("flow_layout  (spacing=1, row_spacing=1)");
    fr->set_border_color(color::green);

    auto * fl = new h_flow(fr);
    fl->set_spacing(1);
    fl->set_row_spacing(1);
    fl->set_margin(1);

    const char * tags[] = {
        "v_box", "h_box", "v_stack", "h_stack", "a_box",
        "flow_layout", "grid_layout", "tab_layout", "free_layout",
        "widget", "label", "button", "line_edit", "frame",
        "scroll_area", "painter", "display_buffer", "color",
        "style_option", "key_event", "mouse_event", "size_policy",
        "screen", "input_reader", "escape_parser",
    };

    for (int i = 0; i < static_cast<int>(std::size(tags)); ++i) {
        const std::string txt = std::string(" ") + tags[i] + " ";
        auto * t = new label(fl);
        t->set_text(txt);
        t->set_fixed_height(1);
        t->set_fixed_width(static_cast<int>(txt.size()));
        t->set_style(fg(color::black) | bg(palette(i)));
    }

    return page;
}

// Page 4: grid_layout — settings form
static widget * make_page_grid(nx::core::object * parent)
{
    auto * page = new v_box(parent);
    page->set_margin(1);

    auto * desc = new label(page);
    desc->set_fixed_height(1);
    desc->set_text("  grid_layout: labels in col 0&2 (fixed), values in col 1&3 (expanding). "
                   "Columns and rows auto-size to content.");
    desc->set_style(fg(color::gray_light));

    auto * fr = new frame(page);
    fr->set_title("grid_layout");
    fr->set_border_color(color::blue);

    auto * gl = new grid(fr);
    gl->set_spacing(1);
    gl->set_margin(2);
    gl->set_column_stretch(1, 1);   // col 1 expands
    gl->set_column_stretch(3, 1);   // col 3 expands

    struct row_def { int r; const char * lbl0; const char * val0;
                          const char * lbl1; const char * val1; };

    const row_def rows[] = {
        { 0, "Name:",    "Jane Doe",        "Age:",     "28"           },
        { 1, "Email:",   "jane@example.com","Phone:",   "+1 555 0100"  },
        { 2, "City:",    "New York",        "Country:", "USA"          },
        { 3, "Role:",    "Engineer",        "Team:",    "nx::tui"      },
        { 4, "Status:",  "Active",          "Level:",   "Senior"       },
    };

    for (auto & rd : rows) {
        auto mk_lbl = [&](const char * t, int col) {
            auto * l = new label(nullptr);
            l->set_text(t);
            l->set_fixed_height(1);
            l->set_style(fg(color::cyan));
            gl->place(l, rd.r, col);
        };
        auto mk_val = [&](const char * t, int col) {
            auto * v = new colored_box(t, color::rgb(30, 30, 60), nullptr);
            v->set_fixed_height(1);
            gl->place(v, rd.r, col);
        };
        mk_lbl(rd.lbl0, 0);
        mk_val(rd.val0, 1);
        mk_lbl(rd.lbl1, 2);
        mk_val(rd.val1, 3);
    }

    return page;
}

// Page 5: tab_layout (nested inside this page)
static widget * make_page_tab(nx::core::object * parent)
{
    auto * page = new v_box(parent);
    page->set_margin(1);

    auto * desc = new label(page);
    desc->set_fixed_height(1);
    desc->set_text("  tab_layout: one child visible at a time.  Click the buttons or use"
                   "  Tab/Shift+Tab to switch sub-tabs.");
    desc->set_style(fg(color::gray_light));

    // Sub-tab bar.
    auto * bar = new h_stack(page);
    bar->set_fixed_height(3);
    bar->set_spacing(1);
    bar->set_margin({0, 0, 0, 0});
    bar->set_alignment(stack_alignment::start);

    // Sub-tab content.
    auto * sub_tabs = new tab_layout(page);

    const char * names[] = { "Alpha", "Beta", "Gamma", "Delta" };
    const color  bgcols[] = {
        color::rgb(60, 20, 80),
        color::rgb(20, 60, 20),
        color::rgb(80, 50, 10),
        color::rgb(20, 50, 80),
    };

    std::vector<button *> sub_btns;

    for (int i = 0; i < 4; ++i) {
        auto * btn = new button(bar);
        btn->set_fixed_width(10);
        sub_btns.push_back(btn);
        btn->set_text(names[i]);

        auto * page_i = new colored_box(
            std::string("  ") + names[i] + " tab content  ", bgcols[i], nullptr);
        sub_tabs->add_tab(page_i);
    }

    // Wire sub-tab controller (stack-allocated, connect via lambdas).
    auto refresh = [sub_btns, sub_tabs]() {
        const int cur = sub_tabs->current();
        for (int i = 0; i < static_cast<int>(sub_btns.size()); ++i) {
            sub_btns[i]->set_style(i == cur
                ? (fg(color::black) | bg(color::yellow))
                : (fg(color::yellow) | bg(color::rgb(30, 30, 30))));
        }
    };
    nx::core::connect(sub_tabs, &tab_layout::current_changed,
                      sub_tabs, [refresh](int) { refresh(); });
    for (int i = 0; i < 4; ++i) {
        nx::core::connect(sub_btns[i], &button::clicked,
                          sub_tabs, [sub_tabs, i]() { sub_tabs->set_current(i); });
    }
    refresh();

    return page;
}

// Page 6: free_layout — absolute positioning
static widget * make_page_free(nx::core::object * parent)
{
    auto * page = new v_box(parent);
    page->set_margin(1);

    auto * desc = new label(page);
    desc->set_fixed_height(2);
    desc->set_text("  free_layout: _apply_layout() is a no-op.  Children keep whatever"
                   " geometry is set on them directly, regardless of parent size.");
    desc->set_style(fg(color::gray_light));

    auto * fr = new frame(page);
    fr->set_title("free_layout");
    fr->set_border_color(color::rgb(180, 80, 200));

    auto * fl = new free_layout(fr);

    struct pin { int x, y, w, h; const char * text; color bg_c; };
    const pin pins[] = {
        {  2,  1, 18,  3, "Pinned at (2,1)",   palette(0) },
        { 25,  4, 20,  3, "Pinned at (25,4)",  palette(1) },
        {  8,  8, 22,  3, "Pinned at (8,8)",   palette(2) },
        { 40,  2, 22,  3, "Pinned at (40,2)",  palette(3) },
        { 30, 10, 20,  3, "Pinned at (30,10)", palette(4) },
    };

    for (auto & p : pins) {
        auto * b = new colored_box(p.text, p.bg_c, fl);
        b->set_geometry({p.x, p.y, p.w, p.h});
    }

    return page;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char * argv[])
{
    tui_application app(argc, argv);
    auto * scr = app.main_screen();
    // scr->set_style(bg(color::rgb(8, 8, 18)));

    // ── root layout ───────────────────────────────────────────────────────────

    auto * root = new v_box(scr);

    // Title bar.
    auto * title = new label(root);
    title->set_fixed_height(1);
    title->set_text("  nx::tui  Layout Demo");
    title->set_style(fg(color::white) | h_gradient_bg(color::rgb(30,30,80), color::rgb(80,20,60)));

    // Tab button bar.
    auto * tab_bar = new h_stack(root);
    tab_bar->set_fixed_height(3);
    tab_bar->set_spacing(1);
    // tab_bar->set_vertical_policy(size_policy::preferred);
    tab_bar->set_alignment(stack_alignment::start);
    tab_bar->set_margin({0, 1, 0, 0});

    const char * page_names[] = {
        "1: v_box/h_box",
        "2: stacks",
        "3: a_box",
        "4: flow",
        "5: grid",
        "6: tab",
        "7: free",
    };

    std::vector<button *> btns;
    for (const char * n : page_names) {
        auto * b = new button(tab_bar);
        b->set_text(n);
        b->set_fixed_width(static_cast<int>(std::string(n).size()) + 2);
        btns.push_back(b);
    }

    // Pages.
    auto * pages = new tab_layout(root);

    pages->add_tab(make_page_boxes(nullptr));
    pages->add_tab(make_page_stacks(nullptr));
    pages->add_tab(make_page_abox(nullptr));
    pages->add_tab(make_page_flow(nullptr));
    pages->add_tab(make_page_grid(nullptr));
    pages->add_tab(make_page_tab(nullptr));
    pages->add_tab(make_page_free(nullptr));

    // Status bar.
    auto * status = new label(root);
    status->set_fixed_height(1);
    status->set_text("  ← → or 1–7: switch page   Tab: focus cycle   Ctrl+C: quit");
    status->set_style(fg(color::gray_light) | bg(color::rgb(15, 15, 25)));

    // ── wiring ────────────────────────────────────────────────────────────────

    tab_controller ctrl(btns, pages);

    app_key_filter key_filter(&ctrl);
    app.install_event_filter(&key_filter);

    return app.exec();
}

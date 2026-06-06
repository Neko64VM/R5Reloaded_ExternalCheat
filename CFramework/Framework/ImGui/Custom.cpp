#include "Custom.h"
#include "imgui_internal.h"
#include <unordered_map>

namespace Custom {

    static float s_tc_sep_x;
    static float s_tc_top_y;
    static float s_tc_bot_y;
    static float s_tc_col_w;
    static float s_tc_gap;
    static float s_tc_sec_top_y;
    static float s_tc_sec_bot_y;

    // --- Utilities -----------------------------------------------------------

    static ImVec4 RGBA(int r, int g, int b, int a = 255) {
        return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    static ImU32 Col(const ImVec4& v) {
        return ImGui::ColorConvertFloat4ToU32(v);
    }

    static ImVec4 Lerp4(const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        );
    }

    static ImVec4 WithA(const ImVec4& c, float a) {
        return ImVec4(c.x, c.y, c.z, a);
    }

    // --- Theme definitions ---------------------------------------------------

    const Theme PinkTheme = {
        RGBA( 18,  18,  22),
        RGBA( 24,  24,  30),
        RGBA( 36,  36,  44),
        RGBA( 50,  50,  62),
        RGBA(255,  72, 150),
        RGBA(255,  72, 150,  80),
        RGBA(238, 238, 248),
        RGBA(150, 150, 168),
        RGBA( 48,  48,  60),
        6.f
    };

    const Theme CyanTheme = {
        RGBA( 13,  15,  22),
        RGBA( 18,  22,  34),
        RGBA( 28,  34,  52),
        RGBA( 40,  48,  70),
        RGBA( 72, 195, 255),
        RGBA( 72, 195, 255,  80),
        RGBA(228, 238, 255),
        RGBA(130, 150, 185),
        RGBA( 36,  44,  66),
        6.f
    };

    // --- Animation -----------------------------------------------------------
    // Exponential lerp toward target, keyed by ImGuiID

    static std::unordered_map<ImGuiID, float> s_anims;

    static float Anim(ImGuiID id, float target, float speed = 10.f) {
        float& v = s_anims[id];
        v += (target - v) * ImMin(speed * ImGui::GetIO().DeltaTime, 1.f);
        return v;
    }

    // --- Page transition state -----------------------------------------------

    static int   s_displayed_tab = -1;
    static int   s_pending_tab   = -1;
    static float s_page_alpha    = 1.f;
    static bool  s_fading_out    = false;

    static void UpdatePageTransition(int* current_tab) {
        const float dt = ImGui::GetIO().DeltaTime;
        if (s_displayed_tab == -1) { s_displayed_tab = *current_tab; s_pending_tab = *current_tab; }
        if (*current_tab != s_pending_tab) { s_pending_tab = *current_tab; s_fading_out = true; }
        if (s_fading_out) {
            s_page_alpha -= dt * 8.f;
            if (s_page_alpha <= 0.f) { s_page_alpha = 0.f; s_displayed_tab = s_pending_tab; s_fading_out = false; }
        } else {
            s_page_alpha = ImMin(s_page_alpha + dt * 7.f, 1.f);
        }
    }

    // --- PushTheme / PopTheme ------------------------------------------------

    void PushTheme(const Theme& t) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg,              t.bg_base);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,               t.bg_panel);
        ImGui::PushStyleColor(ImGuiCol_PopupBg,               t.bg_panel);
        ImGui::PushStyleColor(ImGuiCol_Border,                t.separator);
        ImGui::PushStyleColor(ImGuiCol_FrameBg,               t.bg_widget);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,        t.bg_widget_hovered);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive,         t.bg_widget_hovered);
        ImGui::PushStyleColor(ImGuiCol_TitleBg,               t.bg_panel);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive,         t.bg_panel);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,      t.bg_base);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,           t.bg_base);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,         t.bg_widget);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered,  t.bg_widget_hovered);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive,   t.accent_dim);
        ImGui::PushStyleColor(ImGuiCol_Text,                  t.text);
        ImGui::PushStyleColor(ImGuiCol_TextDisabled,          t.text_dim);
        ImGui::PushStyleColor(ImGuiCol_Header,                t.bg_widget);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,         t.bg_widget_hovered);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,          t.accent_dim);
        ImGui::PushStyleColor(ImGuiCol_Separator,             t.separator);
        // 20 colors

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,    t.rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,     t.rounding * 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding,      t.rounding * 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_TabRounding,       t.rounding * 0.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,  1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,   0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,       ImVec2(8.f, 10.f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,     ImVec2(14.f, 14.f));
        // 8 vars
    }

    void PopTheme() {
        ImGui::PopStyleColor(20);
        ImGui::PopStyleVar(8);
    }

    // --- BeginTabBar ---------------------------------------------------------

    void BeginTabBar(const char* const* labels, int count, int* current_tab, const Theme& t) {
        UpdatePageTransition(current_tab);

        ImDrawList* dl    = ImGui::GetWindowDrawList();
        float       avail = ImGui::GetContentRegionAvail().x;
        float       tab_w = avail / (float)count;
        const float tab_h = 34.f;
        ImVec2      p     = ImGui::GetCursorScreenPos();

        dl->AddRectFilled(p, ImVec2(p.x + avail, p.y + tab_h), Col(t.bg_panel));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));

        for (int i = 0; i < count; i++) {
            ImVec2 tl = ImVec2(p.x + tab_w * i, p.y);
            ImVec2 br = ImVec2(tl.x + tab_w,    tl.y + tab_h);
            bool   is_active = (*current_tab == i);

            ImGui::SetCursorScreenPos(tl);
            ImGui::PushID(i);
            ImGui::InvisibleButton("##tab", ImVec2(tab_w, tab_h));
            bool hov = ImGui::IsItemHovered();
            if (ImGui::IsItemClicked()) *current_tab = i;

            ImGuiID hov_id = ImGui::GetID("h");
            ImGuiID act_id = ImGui::GetID("a");
            ImGui::PopID();

            float hov_t = Anim(hov_id, (hov || is_active) ? 1.f : 0.f, 12.f);
            float act_t = Anim(act_id, is_active           ? 1.f : 0.f, 10.f);

            // Tab background
            ImVec4 bg = Lerp4(t.bg_panel, t.bg_widget, hov_t * 0.5f);
            dl->AddRectFilled(tl, br, Col(bg));

            // Label
            ImVec4 tc  = Lerp4(t.text_dim, t.text, hov_t * 0.5f + act_t * 0.5f);
            ImVec2 tsz = ImGui::CalcTextSize(labels[i]);
            dl->AddText(
                ImVec2((tl.x + br.x) * 0.5f - tsz.x * 0.5f,
                       (tl.y + br.y) * 0.5f - tsz.y * 0.5f),
                Col(tc), labels[i]
            );

            // Active indicator line + glow
            if (act_t > 0.01f) {
                float iw = tab_w * 0.55f * act_t;
                float ix = (tl.x + br.x) * 0.5f - iw * 0.5f;
                dl->AddRectFilled(
                    ImVec2(ix, br.y - 2.f), ImVec2(ix + iw, br.y),
                    Col(WithA(t.accent, act_t)), 1.f
                );
                for (int g = 1; g <= 4; g++) {
                    dl->AddRectFilled(
                        ImVec2(ix - g, br.y - 2.f - g * 2.f),
                        ImVec2(ix + iw + g, br.y),
                        Col(WithA(t.accent_dim, act_t * 0.08f)), 2.f
                    );
                }
            }

            // Vertical separator
            if (i < count - 1) {
                dl->AddLine(
                    ImVec2(br.x, tl.y + 7.f),
                    ImVec2(br.x, br.y - 7.f),
                    Col(t.separator)
                );
            }
        }

        // Bottom border
        dl->AddLine(
            ImVec2(p.x, p.y + tab_h),
            ImVec2(p.x + avail, p.y + tab_h),
            Col(t.separator)
        );

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + tab_h));
        ImGui::Dummy(ImVec2(avail, 10.f));
        ImGui::PopStyleVar();
    }

    void EndTabBar() {}

    // --- BeginTabContent / EndTabContent -------------------------------------

    bool BeginTabContent(int tab_index) {
        if (tab_index != s_displayed_tab) return false;

        float alpha    = s_page_alpha;
        float offset_y = s_fading_out ? 0.f : (1.f - alpha) * (1.f - alpha) * 20.f;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);
        ImGui::BeginGroup();
        return true;
    }

    void EndTabContent() {
        ImGui::EndGroup();
        ImGui::PopStyleVar();
    }

    // --- Button --------------------------------------------------------------

    bool Button(const char* label, const Theme& t, ImVec2 size) {
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2      pos = ImGui::GetCursorScreenPos();
        if (size.x <= 0.f) size.x = ImGui::GetContentRegionAvail().x;
        if (size.y <= 0.f) size.y = 30.f;

        ImGui::InvisibleButton(label, size);
        bool    hov     = ImGui::IsItemHovered();
        bool    act     = ImGui::IsItemActive();
        bool    clicked = ImGui::IsItemClicked();
        ImGuiID id      = ImGui::GetItemID();

        float hov_t = Anim(id,     (hov || act) ? 1.f : 0.f, 12.f);
        float act_t = Anim(id + 1, act           ? 1.f : 0.f, 22.f);

        ImVec2 br = ImVec2(pos.x + size.x, pos.y + size.y);
        ImVec2 c  = ImVec2((pos.x + br.x) * 0.5f, (pos.y + br.y) * 0.5f);

        ImVec4 bg = Lerp4(t.bg_widget, t.bg_widget_hovered, hov_t * 0.7f);
        bg = Lerp4(bg, t.accent, act_t * 0.1f);
        dl->AddRectFilled(pos, br, Col(bg), t.rounding);

        ImVec4 brd = Lerp4(t.separator, t.accent, hov_t);
        dl->AddRect(pos, br, Col(brd), t.rounding, 0, 1.f);

        // Outer glow on hover
        if (hov_t > 0.05f) {
            for (float s = 2.f; s <= 6.f; s += 2.f) {
                dl->AddRect(
                    ImVec2(pos.x - s, pos.y - s),
                    ImVec2(br.x + s,  br.y + s),
                    Col(WithA(t.accent_dim, hov_t * 0.06f)),
                    t.rounding + s
                );
            }
        }

        ImVec4 tc  = Lerp4(t.text_dim, t.text, hov_t);
        ImVec2 tsz = ImGui::CalcTextSize(label);
        dl->AddText(ImVec2(c.x - tsz.x * 0.5f, c.y - tsz.y * 0.5f), Col(tc), label);

        return clicked;
    }

    // --- Toggle --------------------------------------------------------------

    bool Toggle(const char* label, bool* v, const Theme& t) {
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2      pos = ImGui::GetCursorScreenPos();
        const float h = 22.f, w = 44.f, r = h * 0.5f;

        ImGui::InvisibleButton(label, ImVec2(w, h));
        bool    hov     = ImGui::IsItemHovered();
        bool    clicked = ImGui::IsItemClicked();
        if (clicked) *v = !(*v);
        ImGuiID id = ImGui::GetItemID();

        float on_t  = Anim(id,     *v  ? 1.f : 0.f, 10.f);
        float hov_t = Anim(id + 1, hov ? 1.f : 0.f, 14.f);

        ImVec2 br = ImVec2(pos.x + w, pos.y + h);

        ImVec4 track = Lerp4(t.bg_widget, t.accent, on_t * 0.85f + hov_t * 0.08f);
        dl->AddRectFilled(pos, br, Col(track), r);
        ImVec4 brd = Lerp4(t.separator, t.accent, on_t * 0.7f + hov_t * 0.3f);
        dl->AddRect(pos, br, Col(brd), r, 0, 1.f);

        float kx = pos.x + r + (w - h) * on_t;
        float ky  = pos.y + r;
        if (on_t > 0.05f)
            dl->AddCircleFilled(ImVec2(kx, ky), r + 1.f,
                                Col(WithA(t.accent_dim, on_t * 0.4f)));
        dl->AddCircleFilled(ImVec2(kx, ky), r - 2.5f, Col(Lerp4(t.text_dim, t.text, on_t)));
        dl->AddCircle(      ImVec2(kx, ky), r - 2.5f, Col(WithA(t.accent, on_t * 0.5f)), 0, 1.f);

        ImGui::SameLine(0.f, 10.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (h - ImGui::GetTextLineHeight()) * 0.5f);
        ImVec4 tc = Lerp4(t.text_dim, t.text, on_t * 0.4f + hov_t * 0.6f);
        ImGui::PushStyleColor(ImGuiCol_Text, tc);
        ImGui::TextUnformatted(label);
        if (ImGui::IsItemClicked()) { *v = !(*v); clicked = true; }
        ImGui::PopStyleColor();

        return clicked;
    }

    // --- Checkbox ------------------------------------------------------------

    bool Checkbox(const char* label, bool* v, const Theme& t) {
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2      pos = ImGui::GetCursorScreenPos();
        const float sz  = 18.f;

        ImGui::InvisibleButton(label, ImVec2(sz, sz));
        bool    hov     = ImGui::IsItemHovered();
        bool    clicked = ImGui::IsItemClicked();
        if (clicked) *v = !(*v);
        ImGuiID id = ImGui::GetItemID();

        float chk_t = Anim(id,     *v  ? 1.f : 0.f, 12.f);
        float hov_t = Anim(id + 1, hov ? 1.f : 0.f, 14.f);

        ImVec2 br = ImVec2(pos.x + sz, pos.y + sz);

        ImVec4 bg = Lerp4(t.bg_widget, t.bg_widget_hovered, hov_t * 0.5f);
        bg = Lerp4(bg, t.accent, chk_t * 0.3f);
        dl->AddRectFilled(pos, br, Col(bg), 3.f);
        ImVec4 brd = Lerp4(t.separator, t.accent, chk_t * 0.8f + hov_t * 0.4f);
        dl->AddRect(pos, br, Col(brd), 3.f, 0, 1.2f);

        // Two-stroke checkmark drawn sequentially
        if (chk_t > 0.01f) {
            const float pad = 3.f;
            ImVec2 p1 = ImVec2(pos.x + pad,        pos.y + sz * 0.52f);
            ImVec2 p2 = ImVec2(pos.x + sz * 0.42f, pos.y + sz - pad - 1.f);
            ImVec2 p3 = ImVec2(pos.x + sz - pad,   pos.y + pad);
            float  t1 = ImMin(chk_t * 2.f, 1.f);
            float  t2 = ImMax(chk_t * 2.f - 1.f, 0.f);
            ImU32  cc = Col(WithA(t.accent, chk_t));
            if (t1 > 0.f)
                dl->AddLine(p1, ImVec2(p1.x + (p2.x-p1.x)*t1, p1.y + (p2.y-p1.y)*t1), cc, 2.f);
            if (t2 > 0.f)
                dl->AddLine(p2, ImVec2(p2.x + (p3.x-p2.x)*t2, p2.y + (p3.y-p2.y)*t2), cc, 2.f);
        }

        ImGui::SameLine(0.f, 8.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (sz - ImGui::GetTextLineHeight()) * 0.5f);
        ImVec4 tc = Lerp4(t.text_dim, t.text, chk_t * 0.4f + hov_t * 0.6f);
        ImGui::PushStyleColor(ImGuiCol_Text, tc);
        ImGui::TextUnformatted(label);
        if (ImGui::IsItemClicked()) { *v = !(*v); clicked = true; }
        ImGui::PopStyleColor();

        return clicked;
    }

    // --- Combo ---------------------------------------------------------------

    static std::unordered_map<ImGuiID, bool> s_combo_open;

    bool Combo(const char* label, int* current_item, const char* const* items,
               int items_count, const Theme& t) {
        ImDrawList* fg      = ImGui::GetForegroundDrawList();
        ImDrawList* dl      = ImGui::GetWindowDrawList();
        ImVec2      pos     = ImGui::GetCursorScreenPos();
        float       avail_w = ImGui::GetContentRegionAvail().x;
        ImGuiIO&    io      = ImGui::GetIO();

        const float btn_h  = 36.f;
        const float item_h = 32.f;
        const float pad_x  = 12.f;
        const float lh     = ImGui::GetTextLineHeight();
        const float btn_w  = avail_w * 0.5f;

        // Label above the button
        dl->AddText(pos, Col(t.text_dim), label);
        ImGui::Dummy(ImVec2(avail_w, lh + 4.f));
        pos = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton(label, ImVec2(btn_w, btn_h));
        bool    hov     = ImGui::IsItemHovered();
        bool    clicked = ImGui::IsItemClicked();
        ImGuiID id      = ImGui::GetItemID();

        bool& is_open = s_combo_open[id];
        if (clicked) is_open = !is_open;

        float open_t = Anim(id,     is_open ? 1.f : 0.f, 14.f);
        float hov_t  = Anim(id + 1, hov     ? 1.f : 0.f, 14.f);

        // Button background
        ImVec2 br = ImVec2(pos.x + btn_w, pos.y + btn_h);
        ImVec4 bg = Lerp4(t.bg_widget, t.bg_widget_hovered,
                          hov_t * 0.4f + (is_open ? 0.15f : 0.f));
        dl->AddRectFilled(pos, br, Col(bg), t.rounding);
        ImVec4 brd = Lerp4(t.separator, t.accent, hov_t * 0.3f + open_t * 0.7f);
        dl->AddRect(pos, br, Col(brd), t.rounding, 0, 1.2f);

        if (open_t > 0.05f) {
            for (float s = 2.f; s <= 4.f; s += 2.f)
                dl->AddRect(ImVec2(pos.x - s, pos.y - s), ImVec2(br.x + s, br.y + s),
                            Col(WithA(t.accent_dim, open_t * 0.07f)), t.rounding + s);
        }

        // Current item text (centered vertically)
        const char* cur_text = (*current_item >= 0 && *current_item < items_count)
                               ? items[*current_item] : "";
        ImVec2 tsz = ImGui::CalcTextSize(cur_text);
        dl->AddText(ImVec2(pos.x + pad_x, pos.y + (btn_h - tsz.y) * 0.5f),
                    Col(t.text), cur_text);

        // Animated chevron: open_t=0 → ▼, open_t=1 → ▲
        const float as_ = 4.5f;
        float ax = br.x - 20.f, ay = pos.y + btn_h * 0.5f;
        ImVec2 arr_l = ImVec2(ax - as_, ay - as_ * 0.5f + open_t * as_);
        ImVec2 arr_r = ImVec2(ax + as_, ay - as_ * 0.5f + open_t * as_);
        ImVec2 arr_c = ImVec2(ax,       ay + as_ * 0.5f - open_t * as_);
        dl->AddTriangleFilled(arr_l, arr_r, arr_c,
            Col(Lerp4(t.text_dim, t.accent, open_t * 0.6f + hov_t * 0.4f)));

        // Dropdown panel (foreground, clips to animated height)
        bool changed = false;
        if (open_t > 0.005f) {
            float full_h  = items_count * item_h + 8.f;
            float shown_h = full_h * ImMin(open_t * 1.2f, 1.f);

            ImVec2 dtl      = ImVec2(pos.x,          pos.y + btn_h + 3.f);
            ImVec2 dbr_clip = ImVec2(pos.x + btn_w,  dtl.y + shown_h);
            ImVec2 dbr_full = ImVec2(pos.x + btn_w,  dtl.y + full_h);

            fg->PushClipRect(dtl, dbr_clip, true);

            fg->AddRectFilled(dtl, dbr_full, Col(t.bg_panel), t.rounding);
            fg->AddRect(dtl, dbr_full, Col(t.separator), t.rounding, 0, 1.f);
            // Accent line at top of dropdown
            fg->AddLine(ImVec2(dtl.x + t.rounding, dtl.y),
                        ImVec2(dbr_full.x - t.rounding, dtl.y),
                        Col(WithA(t.accent, open_t * 0.6f)), 1.f);

            ImVec2 mouse   = io.MousePos;
            bool   mc      = io.MouseClicked[0];
            bool   in_btn  = mouse.x >= pos.x && mouse.x < br.x &&
                             mouse.y >= pos.y && mouse.y < br.y;
            bool   in_drop = mouse.x >= dtl.x && mouse.x < dbr_clip.x &&
                             mouse.y >= dtl.y && mouse.y < dbr_clip.y;

            for (int i = 0; i < items_count; i++) {
                float  iy  = dtl.y + 4.f + i * item_h;
                ImVec2 itl = ImVec2(dtl.x + 4.f,          iy);
                ImVec2 ibr = ImVec2(dbr_full.x - 4.f, iy + item_h - 2.f);

                bool item_hov = is_open &&
                    mouse.x >= itl.x && mouse.x < ibr.x &&
                    mouse.y >= iy    && mouse.y < iy + item_h;

                ImGuiID iid    = id + 100 + (ImGuiID)i;
                float   ihov_t = Anim(iid,       item_hov             ? 1.f : 0.f, 16.f);
                float   isel_t = Anim(iid + 200, (*current_item == i) ? 1.f : 0.f, 10.f);

                if (ihov_t > 0.01f || isel_t > 0.01f)
                    fg->AddRectFilled(itl, ibr,
                        Col(WithA(t.accent, ihov_t * 0.18f + isel_t * 0.08f)),
                        t.rounding * 0.5f);

                // Selected indicator bar
                if (isel_t > 0.01f)
                    fg->AddRectFilled(
                        ImVec2(itl.x, itl.y + 4.f),
                        ImVec2(itl.x + 3.f, ibr.y - 4.f),
                        Col(WithA(t.accent, isel_t)), 1.5f);

                ImVec2 itsz = ImGui::CalcTextSize(items[i]);
                fg->AddText(
                    ImVec2(itl.x + pad_x, iy + ((item_h - 2.f) - itsz.y) * 0.5f),
                    Col(Lerp4(t.text_dim, t.text, ihov_t * 0.5f + isel_t * 0.5f)),
                    items[i]);

                if (item_hov && mc) {
                    *current_item      = i;
                    is_open            = false;
                    changed            = true;
                    io.MouseClicked[0] = false;
                }
            }

            fg->PopClipRect();

            if (mc && !changed && !in_btn && !in_drop)
                is_open = false;
        }

        return changed;
    }

    // --- SliderFloat ---------------------------------------------------------

    bool SliderFloat(const char* label, float* v, float v_min, float v_max,
                     const Theme& t, const char* fmt) {
        ImDrawList* dl      = ImGui::GetWindowDrawList();
        ImVec2      pos     = ImGui::GetCursorScreenPos();
        float       avail_w = ImGui::GetContentRegionAvail().x;

        const float lh       = ImGui::GetTextLineHeight();
        const float pad_x    = 8.f;
        const float track_h  = 4.f;
        const float knob_r   = 7.f;
        const float widget_h = lh + 10.f + knob_r * 2.f;

        dl->AddText(pos, Col(t.text_dim), label);

        char val_buf[32];
        snprintf(val_buf, sizeof(val_buf), fmt, *v);
        ImVec2 vsz = ImGui::CalcTextSize(val_buf);
        dl->AddText(ImVec2(pos.x + avail_w - vsz.x, pos.y), Col(t.accent), val_buf);

        float track_y   = pos.y + lh + 10.f + knob_r;
        float track_x1  = pos.x + pad_x;
        float track_x2  = pos.x + avail_w - pad_x;
        float track_len = track_x2 - track_x1;

        ImGui::InvisibleButton(label, ImVec2(avail_w, widget_h));
        bool    hov     = ImGui::IsItemHovered();
        bool    act     = ImGui::IsItemActive();
        bool    changed = false;
        ImGuiID id      = ImGui::GetItemID();

        if (act) {
            float mx = ImClamp(ImGui::GetIO().MousePos.x, track_x1, track_x2);
            *v = v_min + ((mx - track_x1) / track_len) * (v_max - v_min);
            changed = true;
        }

        float ratio  = ImClamp((*v - v_min) / (v_max - v_min), 0.f, 1.f);
        float knob_x = track_x1 + track_len * ratio;

        float hov_t = Anim(id,     (hov || act) ? 1.f : 0.f, 12.f);
        float act_t = Anim(id + 1, act           ? 1.f : 0.f, 20.f);

        // Track background
        dl->AddRectFilled(
            ImVec2(track_x1, track_y - track_h * 0.5f),
            ImVec2(track_x2, track_y + track_h * 0.5f),
            Col(t.bg_widget), track_h
        );
        // Progress fill
        dl->AddRectFilled(
            ImVec2(track_x1, track_y - track_h * 0.5f),
            ImVec2(knob_x,   track_y + track_h * 0.5f),
            Col(t.accent), track_h
        );

        // Knob (grows slightly on hover)
        float r = knob_r * (1.f + hov_t * 0.15f + act_t * 0.05f);
        dl->AddCircleFilled(ImVec2(knob_x, track_y), r,          Col(t.bg_widget));
        dl->AddCircle(      ImVec2(knob_x, track_y), r,          Col(t.accent), 0, 1.5f);
        dl->AddCircleFilled(ImVec2(knob_x, track_y), r * 0.35f,  Col(t.accent));

        return changed;
    }

    // --- SliderInt -----------------------------------------------------------

    bool SliderInt(const char* label, int* v, int v_min, int v_max,
                   const Theme& t, const char* fmt) {
        ImDrawList* dl      = ImGui::GetWindowDrawList();
        ImVec2      pos     = ImGui::GetCursorScreenPos();
        float       avail_w = ImGui::GetContentRegionAvail().x;

        const float lh       = ImGui::GetTextLineHeight();
        const float pad_x    = 8.f;
        const float track_h  = 4.f;
        const float knob_r   = 7.f;
        const float widget_h = lh + 10.f + knob_r * 2.f;

        dl->AddText(pos, Col(t.text_dim), label);

        char val_buf[32];
        snprintf(val_buf, sizeof(val_buf), fmt, *v);
        ImVec2 vsz = ImGui::CalcTextSize(val_buf);
        dl->AddText(ImVec2(pos.x + avail_w - vsz.x, pos.y), Col(t.accent), val_buf);

        float track_y   = pos.y + lh + 10.f + knob_r;
        float track_x1  = pos.x + pad_x;
        float track_x2  = pos.x + avail_w - pad_x;
        float track_len = track_x2 - track_x1;

        ImGui::InvisibleButton(label, ImVec2(avail_w, widget_h));
        bool    hov     = ImGui::IsItemHovered();
        bool    act     = ImGui::IsItemActive();
        bool    changed = false;
        ImGuiID id      = ImGui::GetItemID();

        if (act) {
            float mx  = ImClamp(ImGui::GetIO().MousePos.x, track_x1, track_x2);
            float raw = v_min + ((mx - track_x1) / track_len) * (float)(v_max - v_min);
            int   nv  = (int)(raw + 0.5f);
            nv = ImClamp(nv, v_min, v_max);
            if (nv != *v) { *v = nv; changed = true; }
        }

        float ratio  = (v_max > v_min)
                       ? ImClamp((float)(*v - v_min) / (float)(v_max - v_min), 0.f, 1.f)
                       : 0.f;
        float knob_x = track_x1 + track_len * ratio;

        float hov_t = Anim(id,     (hov || act) ? 1.f : 0.f, 12.f);
        float act_t = Anim(id + 1, act           ? 1.f : 0.f, 20.f);

        dl->AddRectFilled(
            ImVec2(track_x1, track_y - track_h * 0.5f),
            ImVec2(track_x2, track_y + track_h * 0.5f),
            Col(t.bg_widget), track_h
        );
        dl->AddRectFilled(
            ImVec2(track_x1, track_y - track_h * 0.5f),
            ImVec2(knob_x,   track_y + track_h * 0.5f),
            Col(t.accent), track_h
        );

        float r = knob_r * (1.f + hov_t * 0.15f + act_t * 0.05f);
        dl->AddCircleFilled(ImVec2(knob_x, track_y), r,         Col(t.bg_widget));
        dl->AddCircle(      ImVec2(knob_x, track_y), r,         Col(t.accent), 0, 1.5f);
        dl->AddCircleFilled(ImVec2(knob_x, track_y), r * 0.35f, Col(t.accent));

        return changed;
    }

    // --- SectionHeader -------------------------------------------------------

    void SectionHeader(const char* label, const Theme& t) {
        ImDrawList* dl      = ImGui::GetWindowDrawList();
        ImVec2      pos     = ImGui::GetCursorScreenPos();
        float       avail_w = ImGui::GetContentRegionAvail().x;
        float       h       = ImGui::GetTextLineHeight();

        dl->AddRectFilled(
            ImVec2(pos.x,       pos.y + 2.f),
            ImVec2(pos.x + 3.f, pos.y + h - 2.f),
            Col(t.accent), 1.5f
        );

        dl->AddText(ImVec2(pos.x + 8.f, pos.y), Col(t.text), label);

        ImVec2 tsz    = ImGui::CalcTextSize(label);
        float  line_x = pos.x + 8.f + tsz.x + 8.f;
        float  line_y = pos.y + h * 0.55f;
        dl->AddLine(ImVec2(line_x, line_y), ImVec2(pos.x + avail_w, line_y),
                    Col(t.separator), 1.f);

        ImGui::Dummy(ImVec2(avail_w, h + 4.f));
    }

    // --- Separator -----------------------------------------------------------

    void Separator(const Theme& t) {
        ImDrawList* dl      = ImGui::GetWindowDrawList();
        ImVec2      pos     = ImGui::GetCursorScreenPos();
        float       avail_w = ImGui::GetContentRegionAvail().x;
        dl->AddLine(pos, ImVec2(pos.x + avail_w, pos.y), Col(t.separator), 1.f);
        ImGui::Dummy(ImVec2(avail_w, 1.f));
    }

    // --- BeginSection / EndSection -------------------------------------------

    void BeginSection(const char* label, const Theme& t) {
        ImGui::Dummy(ImVec2(0.f, 4.f));
        ImVec4 mid_bg = Lerp4(t.bg_base, t.bg_panel, 0.5f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,  mid_bg);
        ImGui::PushStyleColor(ImGuiCol_Border,   t.separator);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,   t.rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,   ImVec2(10.f, 10.f));
        float avail_h = ImGui::GetContentRegionAvail().y;
        s_tc_sec_top_y = ImGui::GetCursorScreenPos().y;
        s_tc_sec_bot_y = s_tc_sec_top_y + avail_h;
        ImGui::BeginChild(label, ImVec2(0.f, avail_h),
            ImGuiChildFlags_Borders,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        SectionHeader(label, t);
    }

    void EndSection(const Theme& t) {
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

    // --- BeginSideNav / EndSideNav -------------------------------------------

    void BeginSideNav(const char* brand, float width, const Theme& t) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, t.bg_panel);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 8.f));
        ImGui::BeginChild("##sidenav", ImVec2(width, 0.f), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Right border at 'width', not width+gap
        ImDrawList* dl   = ImGui::GetWindowDrawList();
        ImVec2      wpos = ImGui::GetWindowPos();
        ImVec2      wsz  = ImGui::GetWindowSize();
        dl->AddLine(
            ImVec2(wpos.x + width - 1.f, wpos.y),
            ImVec2(wpos.x + width - 1.f, wpos.y + wsz.y),
            Col(t.separator), 1.f
        );

        // Brand centered within 'width' (excluding gap)
        float  avail = width;
        ImVec2 tsz   = ImGui::CalcTextSize(brand);
        ImGui::SetCursorPosX((avail - tsz.x) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, t.accent);
        ImGui::TextUnformatted(brand);
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0.f, 6.f));

        // Separator under brand within 'width'
        ImVec2 sp = ImGui::GetCursorScreenPos();
        dl->AddLine(sp, ImVec2(sp.x + avail, sp.y), Col(t.separator), 1.f);
        ImGui::Dummy(ImVec2(avail, 1.f));
        ImGui::Dummy(ImVec2(0.f, 6.f));
    }

    void EndSideNav() {
        ImGui::EndChild();
        ImGui::PopStyleVar();   // WindowPadding
        ImGui::PopStyleColor(); // ChildBg
    }

    // --- SideNavGroup ---------------------------------------------------------

    void SideNavGroup(const char* label, const Theme& t) {
        ImGui::Dummy(ImVec2(0.f, 4.f));

        ImFont*     font   = ImGui::GetFont();
        float       sm_sz  = ImGui::GetFontSize() * 0.75f;
        ImDrawList* dl     = ImGui::GetWindowDrawList();
        ImVec2      pos    = ImGui::GetCursorScreenPos();
        pos.x += 10.f;
        dl->AddText(font, sm_sz, pos, Col(t.text_dim), label);

        ImVec2 tsz = font->CalcTextSizeA(sm_sz, FLT_MAX, 0.f, label);
        ImGui::Dummy(ImVec2(tsz.x + 10.f, tsz.y));
        ImGui::Dummy(ImVec2(0.f, 2.f));
    }

    // --- SideNavItem ----------------------------------------------------------

    bool SideNavItem(const char* label, int index, int* current_tab, const Theme& t, const char* icon) {
        bool        is_active = (*current_tab == index);
        const float item_h   = 30.f;
        float       avail_w  = ImGui::GetContentRegionAvail().x;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(label, ImVec2(avail_w, item_h));
        bool    hov     = ImGui::IsItemHovered();
        bool    clicked = ImGui::IsItemClicked();
        if (clicked) *current_tab = index;
        ImGuiID id = ImGui::GetItemID();

        float hov_t = Anim(id,     (hov || is_active) ? 1.f : 0.f, 12.f);
        float act_t = Anim(id + 1, is_active           ? 1.f : 0.f, 10.f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2      br = ImVec2(pos.x + avail_w, pos.y + item_h);

        // Hover/active background
        if (hov_t > 0.01f || act_t > 0.01f) {
            float bg_a = act_t * 0.2f + hov_t * 0.08f;
            dl->AddRectFilled(pos, br, Col(WithA(t.accent, bg_a)), t.rounding * 0.5f);
        }

        // Left accent bar with glow
        if (act_t > 0.01f) {
            float bar_h = item_h * 0.55f;
            float bar_y = pos.y + (item_h - bar_h) * 0.5f;
            dl->AddRectFilled(
                ImVec2(pos.x, bar_y), ImVec2(pos.x + 3.f, bar_y + bar_h),
                Col(WithA(t.accent, act_t)), 2.f
            );
            dl->AddRectFilled(
                ImVec2(pos.x, bar_y), ImVec2(pos.x + 8.f, bar_y + bar_h),
                Col(WithA(t.accent_dim, act_t * 0.35f)), 2.f
            );
        }

        ImVec4 tc       = Lerp4(t.text_dim, t.text, act_t * 0.6f + hov_t * 0.4f);
        float  text_y   = ImGui::GetFontSize();
        float  base_x   = pos.x + 14.f;
        float  center_y = pos.y + (item_h - text_y) * 0.5f;

        if (icon && icon[0] != '\0') {
            const float icon_slot = 18.f;
            ImVec2 isz = ImGui::CalcTextSize(icon);
            dl->AddText(
                ImVec2(base_x + (icon_slot - isz.x) * 0.5f, center_y),
                Col(WithA(t.accent, 0.65f + act_t * 0.35f)), icon
            );
            dl->AddText(ImVec2(base_x + icon_slot + 6.f, center_y), Col(tc), label);
        } else {
            dl->AddText(ImVec2(base_x, center_y), Col(tc), label);
        }

        return clicked;
    }

    // --- BeginContentArea / EndContentArea ------------------------------------

    void BeginContentArea(int* current_tab, const Theme& t) {
        UpdatePageTransition(current_tab);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, t.bg_base);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.f, 14.f));
        ImGui::BeginChild("##content", ImVec2(0.f, 0.f), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    void EndContentArea() {
        ImGui::EndChild();
        ImGui::PopStyleVar();   // WindowPadding
        ImGui::PopStyleColor(); // ChildBg
    }

    // --- BeginTwoColumnPage / TwoColumnSplit / EndTwoColumnPage ---------------

    bool BeginTwoColumnPage(int tab_index, const Theme& t) {
        if (tab_index != s_displayed_tab)
            return false;

        float alpha    = s_page_alpha;
        float offset_y = s_fading_out ? 0.f : (1.f - alpha) * (1.f - alpha) * 20.f;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);

        float avail_w = ImGui::GetContentRegionAvail().x;
        float avail_h = ImGui::GetContentRegionAvail().y;
        s_tc_gap   = 18.f;
        s_tc_col_w = (avail_w - s_tc_gap) * 0.5f;

        ImVec2 sp  = ImGui::GetCursorScreenPos();
        s_tc_sep_x = sp.x + s_tc_col_w + s_tc_gap * 0.5f;
        s_tc_top_y = sp.y;
        s_tc_bot_y = sp.y + avail_h;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 8.f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::BeginChild("##col0", ImVec2(s_tc_col_w, avail_h), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        return true;
    }

    void TwoColumnSplit(const Theme& t) {
        ImGui::EndChild();
        ImGui::PopStyleColor(); // ChildBg col0
        ImGui::PopStyleVar();   // WindowPadding col0

        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(s_tc_sep_x, s_tc_sec_top_y),
            ImVec2(s_tc_sep_x, s_tc_sec_bot_y),
            Col(t.separator), 1.f
        );

        ImGui::SameLine(0.f, s_tc_gap);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 8.f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::BeginChild("##col1", ImVec2(s_tc_col_w, s_tc_bot_y - s_tc_top_y), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    }

    void EndTwoColumnPage() {
        ImGui::EndChild();
        ImGui::PopStyleColor(); // ChildBg col1
        ImGui::PopStyleVar();   // WindowPadding col1
        ImGui::PopStyleVar();   // Alpha
    }

}

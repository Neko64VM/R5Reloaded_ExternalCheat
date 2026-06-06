#pragma once
#include "imgui.h"

namespace Custom {

    struct Theme {
        ImVec4 bg_base;
        ImVec4 bg_panel;
        ImVec4 bg_widget;
        ImVec4 bg_widget_hovered;
        ImVec4 accent;
        ImVec4 accent_dim;
        ImVec4 text;
        ImVec4 text_dim;
        ImVec4 separator;
        float  rounding;
    };

    // Gray base + pink accent
    extern const Theme PinkTheme;
    // Gray base + cyan/blue accent
    extern const Theme CyanTheme;

    // Apply / restore ImGui colors and style vars
    void PushTheme(const Theme& t);
    void PopTheme();

    // Animated tab bar. Always call EndTabBar.
    void BeginTabBar(const char* const* labels, int count, int* current_tab, const Theme& t);
    void EndTabBar();

    // Wraps tab page content with fade + slide-in animation.
    // Returns false when the page is not active; do NOT call EndTabContent in that case.
    bool BeginTabContent(int tab_index);
    void EndTabContent();

    // Widgets
    bool Button(const char* label, const Theme& t, ImVec2 size = ImVec2(0.f, 0.f));
    bool Toggle(const char* label, bool* v, const Theme& t);
    bool Checkbox(const char* label, bool* v, const Theme& t);
    bool SliderFloat(const char* label, float* v, float v_min, float v_max,
                     const Theme& t, const char* fmt = "%.2f");
    bool SliderInt(const char* label, int* v, int v_min, int v_max,
                   const Theme& t, const char* fmt = "%d");
    bool Combo(const char* label, int* current_item, const char* const* items,
               int items_count, const Theme& t);

    // Decorations
    void SectionHeader(const char* label, const Theme& t);
    void Separator(const Theme& t);
    void BeginSection(const char* label, const Theme& t);
    void EndSection(const Theme& t);

    // Layout: left sidebar navigation + two-column content area
    void BeginSideNav(const char* brand, float width, const Theme& t);
    void EndSideNav();
    void SideNavGroup(const char* label, const Theme& t);
    // icon に nullptr を渡すとアイコンなし（後方互換）
    bool SideNavItem(const char* label, int index, int* current_tab, const Theme& t, const char* icon = nullptr);
    void BeginContentArea(int* current_tab, const Theme& t);
    void EndContentArea();
    bool BeginTwoColumnPage(int tab_index, const Theme& t);
    void TwoColumnSplit(const Theme& t);
    void EndTwoColumnPage();

} // namespace Custom

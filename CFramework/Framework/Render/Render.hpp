#pragma once
#include "../ImGui/imgui.h"
#include "../ImGui/Fonts/fa.h"
#include "../ImGui/Fonts/IconsFontAwesome6.h"
#include <SimpleMath/SimpleMath.h>
using namespace DirectX::SimpleMath;

// ImGuiでの図形描画用クラス.
class CRenderer
{
public:
    ImColor TEXT_COLOR{ 1.f, 1.f, 1.f, 1.f };

    // Init
    ImFont* m_fFont{ nullptr };
    ImFont* m_fIcon{ nullptr };
    void ApplyStyle();

    // Render.cpp
    ImVec2 ToImVec2(const ImVec2& value); // Convert to int
    ImVec2 ToImVec2(const Vector2& value);
    ImColor ApplyAlpha(const ImColor& color, const float& alpha);
    float GetHueFromTime(float speed = 5.0f);
    ImColor GenerateRainbow(float speed = 5.0f);
    void Line(const Vector2 p1, const Vector2 p2, ImColor color, float thickness = 1.f);
    void Circle(const Vector2 pos, float size, ImColor color);
    void CircleFilled(const Vector2 pos, float size, ImColor color, float alpha);
    void Rect(Vector2 min, Vector2 max, ImColor color);
    void CorneredBox(Vector2 min, Vector2 max, int scale, ImColor color);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color);
    void Healthbar(Vector2 min, Vector2 max, int value, int v_max, ImColor shadow_color, float gAlpha);
    void Shieldbar(Vector2 min, Vector2 max, int value, int v_max, ImColor shadow_color, float gAlpha);
    void String(const Vector2 pos, ImColor color, float alpha, const char* text);
    void StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text);
};
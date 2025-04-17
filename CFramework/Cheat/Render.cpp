#include "CFramework.h"

// レンダリングに使う関数をまとめているだけです。わざわざいじる必要はないです。

float CFramework::GetHueFromTime(float speed) {
    float t = static_cast<float>(ImGui::GetTime());
    return fmodf(t / speed, 1.0f);
}

ImColor CFramework::GenerateRainbow(float speed) {
    float hue = GetHueFromTime(speed);
    return ImColor::HSV(hue, 1.0f, 1.0f);
}

ImVec2 CFramework::ToImVec2(const ImVec2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImVec2 CFramework::ToImVec2(const Vector2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImColor CFramework::WithAlpha(const ImColor& color, const float& alpha) {
    return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

// Line
void CFramework::DrawLine(const Vector2 p1, const Vector2 p2, ImColor color, float thickness)
{
    ImGui::GetBackgroundDrawList()->AddLine(ToImVec2(p1), ToImVec2(p2), color, thickness);
}

// Box
void CFramework::DrawBox(int right, int left, int top, int bottom, ImColor color, float thickness)
{
    DrawLine(Vector2(left, top), Vector2(right, top), color, 1.f);
    DrawLine(Vector2(left, top), Vector2(left, bottom), color, 1.f);
    DrawLine(Vector2(right, top), Vector2(right, bottom), color, 1.f);
    DrawLine(Vector2(left, bottom), Vector2(right, bottom), color, 1.f);
}

// Circle
void CFramework::DrawCircle(const Vector2 pos, float size, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ToImVec2(pos), size, color);
}

void CFramework::DrawCircle(const Vector2 pos, float size, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ToImVec2(pos), size, WithAlpha(color, alpha));
}

// CircleFilled
void CFramework::DrawCircleFilled(const Vector2 pos, float size, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ToImVec2(pos), size, WithAlpha(color, alpha));
}

void CFramework::RectFilled(int x0, int y0, int x1, int y1, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(x0, y0)), ToImVec2(Vector2(x1, y1)), color);
}

void CFramework::RectFilled(int x0, int y0, int x1, int y1, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(x0, y0)), ToImVec2(Vector2(x1, y1)), WithAlpha(color, alpha));
}

void CFramework::HealthBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha)
{
    RectFilled(x - 1, y + 1, x + w + 1, y + h - 1, shadow);
    RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, (int)(255 * global_alpha)), global_alpha);
}

void CFramework::ShieldBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha) 
{
    ImColor barColor = ImColor(1.f, 1.f, 1.f, 1.f);

    switch (v_max)
    {
    case 50:  barColor = ImColor(1.f, 1.f, 1.f, 1.f); break;
    case 75: barColor = ImColor(0.f, 0.85f, 1.f, 1.f); break;
    case 100: barColor = ImColor(0.7f, 0.f, 1.f, 1.f); break;
    case 125: barColor = ImColor(1.f, 0.01f, 0.f, 1.f); break;
    default: barColor = GenerateRainbow(); break;
    }

    RectFilled(x, y, x + w, y + h, shadow);
    RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor);
}

void CFramework::String(const Vector2 pos, ImColor color, float alpha, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), WithAlpha(color, alpha), text, text + strlen(text), 1024);
}

void CFramework::StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ToImVec2(Vector2(pos.x + 1.f, pos.y + 1.f)), shadow_color, text, text + strlen(text), 1024, 0); // Shadow
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), WithAlpha(TEXT_COLOR_DEFAULT, global_alpht), text, text + strlen(text), 1024); // Text
}
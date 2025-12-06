#include "CFramework.h"

float CRenderer::GetHueFromTime(float speed) {
    float t = static_cast<float>(ImGui::GetTime());
    return fmodf(t / speed, 1.0f);
}

ImColor CRenderer::GenerateRainbow(float speed) {
    float hue = GetHueFromTime(speed);
    return ImColor::HSV(hue, 1.0f, 1.0f);
}

ImVec2 CRenderer::ToImVec2(const ImVec2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImVec2 CRenderer::ToImVec2(const Vector2& value) {
    return ImVec2((int)value.x, (int)value.y);
}

ImColor CRenderer::ApplyAlpha(const ImColor& color, const float& alpha) {
    return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

void CRenderer::Line(const Vector2 p1, const Vector2 p2, ImColor color, float thickness)
{
    ImGui::GetBackgroundDrawList()->AddLine(ToImVec2(p1), ToImVec2(p2), color, thickness);
}

void CRenderer::Circle(const Vector2 pos, float size, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ToImVec2(pos), size, color);
}

void CRenderer::CircleFilled(const Vector2 pos, float size, ImColor color, float alpha)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ToImVec2(pos), size, ApplyAlpha(color, alpha));
}

void CRenderer::CorneredBox(Vector2 min, Vector2 max, int scale, ImColor color)
{
    Line(Vector2(min.x, min.y), Vector2(min.x + scale, min.y), color); // Top
    Line(Vector2(max.x, min.y), Vector2(max.x - scale, min.y), color);
    Line(Vector2(min.x, min.y), Vector2(min.x, min.y + scale), color); // Left
    Line(Vector2(min.x, max.y), Vector2(min.x, max.y - scale), color);
    Line(Vector2(max.x, min.y), Vector2(max.x, min.y + scale), color); // Right
    Line(Vector2(max.x, max.y), Vector2(max.x, max.y - scale), color);
    Line(Vector2(min.x, max.y), Vector2(min.x + scale, max.y), color); // Bottom
    Line(Vector2(max.x + 1, max.y), Vector2(max.x - scale, max.y), color);
}

void CRenderer::Rect(Vector2 min, Vector2 max, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddRect(ToImVec2(Vector2(min)), ToImVec2(Vector2(max)), color);
}

void CRenderer::RectFilled(int x0, int y0, int x1, int y1, ImColor color)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(x0, y0)), ToImVec2(Vector2(x1, y1)), color);
}

void CRenderer::Healthbar(Vector2 min, Vector2 max, int value, int v_max, ImColor shadow_color, float gAlpha)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(min.x - 1, min.y - 1)), ToImVec2(Vector2(max.x + 1, max.y + 2)), shadow_color);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(min.x, max.y + (((min.y - max.y) / v_max) * value))), ToImVec2(Vector2(max.x, max.y + 1)), ImColor(min(510 * (v_max - value) / 100, 255), min(510 * value / 100, 255), 25, (int)(255 * gAlpha)), gAlpha);
}

void CRenderer::Shieldbar(Vector2 min, Vector2 max, int value, int v_max, ImColor shadow_color, float gAlpha)
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

    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(min.x - 1, min.y - 1)), ToImVec2(Vector2(max.x + 1, max.y + 2)), shadow_color);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ToImVec2(Vector2(min.x, max.y + (((min.y - max.y) / v_max) * value))), ToImVec2(Vector2(max.x, max.y + 1)), barColor, (int)(255 * gAlpha));
}

void CRenderer::String(const Vector2 pos, ImColor color, float alpha, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), ApplyAlpha(color, alpha), text, text + strlen(text), 512);
}

void CRenderer::StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text)
{
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), font_size, ToImVec2(Vector2(pos.x + 1.f, pos.y + 1.f)), shadow_color, text, text + strlen(text), 512, 0); // Shadow
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ToImVec2(pos), ApplyAlpha(TEXT_COLOR, global_alpht), text, text + strlen(text), 512); // Text
}
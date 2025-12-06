#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Fonts/fa.h"
#include "../Framework/ImGui/Fonts/IconsFontAwesome6.h"
#include "../Framework/ImGui/Custom.h"
#include "SDK/CEntity/CEntity.h"
#include <mutex>
#include <atomic>
#pragma comment(lib, "WinMM.lib")
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    void Init();
    void UpdateList();
	void RenderInfo();
	void RenderMenu();
    void RenderESP();
    void SpectatorCrash();
    void MultiFeatures();
private:
    ImFont* m_Icon{ nullptr };
    Vector2 m_screenSize{ Vector2() };
    ImColor COLOR_RADAR_FRAME{ 1.f, 1.f, 1.f, 0.5f };
    ImColor TEXT_COLOR_DEFAULT{ 1.f, 1.f, 1.f, 1.f };
    ImColor TEXT_COLOR_ATTENTION{ 1.f, 1.f, 0.f, 1.f };
    ImColor TEXT_COLOR_WARNING{ 1.f, 0.f, 0.f, 1.f };

    // スレッドセーフを考慮する
    std::mutex mtx;
    CEntity m_CLocal{};
    CEntity m_CLocalCopy{}; // UpdateList() 関数以外で使用する
    std::vector<CEntity> m_CEntityList{};
    std::vector<std::string> m_CSpectatorList{};

    int m_bodySKinId{ -1 };
    int m_weaponSKinId{ -1 };

    // AimBot KeyChecker
    bool AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode);
};

class CRenderer
{
public:
    ImColor TEXT_COLOR{ 1.f, 1.f, 1.f, 1.f };

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
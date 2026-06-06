#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Custom.h"
#include "../Framework/Render/Render.hpp"
#include "../Framework/Config/Config.h"
#include "../Framework/Memory/Memory.h"
#include "SDK/CEntity/CEntity.h"
#include <mutex>
#pragma comment(lib, "WinMM.lib")
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    bool Init();
	void UpdateSnapshot();
    void UpdateList();
	void RenderInfo();
	void RenderMenu();
    void RenderESP();
    // void SpectatorCrash(); // dev
    void MultiFeatures();
private:
	std::unique_ptr<CRenderer> m_gui{ std::make_unique<CRenderer>() };

    Vector2 m_screenSize{ Vector2() };
    ImColor COLOR_RADAR_FRAME{ 1.f, 1.f, 1.f, 0.5f };
    ImColor TEXT_COLOR_DEFAULT{ 1.f, 1.f, 1.f, 1.f };
    ImColor TEXT_COLOR_ATTENTION{ 1.f, 1.f, 0.f, 1.f };
    ImColor TEXT_COLOR_WARNING{ 1.f, 0.f, 0.f, 1.f };

    int m_bodySkinId{ -1 };
    int m_weaponSkinId{ -1 };

    const Custom::Theme& theme = Custom::CyanTheme;

    // AimBot KeyChecker
    bool AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode);

    // スレッドセーフ用
    struct CGameDataSnapshot {
        CEntity m_localplayer{0};
        std::vector<CEntity> m_entitylist{};
        std::vector<std::string> m_spectatorlist{};
    };

    std::mutex m_lock;
    CGameDataSnapshot m_GameDataSnapshot{};
    CGameDataSnapshot m_NextGameDataSnapshot{};
};
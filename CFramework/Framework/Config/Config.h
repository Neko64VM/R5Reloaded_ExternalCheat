#pragma once
#include "../ImGui/imgui.h"
#include <vector>
#include <string>
#include <iostream>
#include <Windows.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <atomic>
#include "../Framework.h"
#include <SimpleMath/SimpleMath.h>
using namespace DirectX::SimpleMath;
using json = nlohmann::json;

// GlobalVars
struct GlobalVars
{
    // System
    bool bShowMenu{ false };

    // KeyBoard Hook/Input
    std::string newConfigName{};
    bool GenerateFlag{ false };

    // Window Data
    HWND hGameWindow{};
    Vector2 gameSize{};

    // KeyData
    DWORD dwMenuKey{ VK_INSERT };
    DWORD dwAimKey0{ VK_RBUTTON };
    DWORD dwAimKey1{ VK_LBUTTON };

    // AimBot
    bool AimBotEnable{ true };
    bool bVisCheck{ true };
    bool bRemoveSway{ true };
    bool bShowFOV{ true };
    bool bRainbowFOV{ false };
    int AimFOV{ 150 };
    float AimSmooth{ 2.5f };
    int AimMode{ 0 };
    int AimKeyMode{ 2 }; // None, and, or
    int AimTargetBone{ 1 };
    int AimMaxDistance{ 100 };

    // Visual
    bool VisualEnable{ true };
    bool RadarEnable{ true };
    int GlowStyle = 0;
    bool ESP_NPC{ true };
    bool ESP_Team{ false };
    bool bBox{ true };
    bool bFilled{ false };
    bool bLine{ false };
    bool bSkeleton{ true };
    bool bHealth{ true };
    bool bDistance{ true };
    bool bName{ true };
    bool bWeapon{ true };
    float RadarScale{ 12.f };
    int ESP_BoxType{ 1 };
    int ESP_BoxRenderMode{ 1 };
    int ESP_MaxDistance{ 500 };

    // Misc
    bool SpectatorListEnable{ true };
    bool CrosshairEnable{ false };
    int CrosshairSize{ 5 };
    int CrosshairType{ 0 };
    int MaxFramerate{ 185 };
    float AimAssistMod{ 0.6f };
    bool RecoilControllSystem{ false };
    float RCS_Scale{ 1.f };
    float VMG_Rate{ 5.f };
    int VMG_Type{ 0 };
    bool bSkinChanger{ false };

    // Colors
    float m_flGlobalAlpha{ 0.925f };
    float m_flShadowAlpha{ 0.235f };
    ImColor Color_ESP_Team{ 0.f, 0.75f, 1.f, m_flGlobalAlpha };
    ImColor Color_ESP_Enemy{ 1.f, 0.f, 0.f, m_flGlobalAlpha };
    ImColor Color_ESP_Visible{ 0.f, 1.f, 0.f, m_flGlobalAlpha };
    ImColor Color_ESP_AimTarget{ 1.f, 1.f, 0.f, m_flShadowAlpha };
    ImColor Color_ESP_Shadow{ 0.f, 0.f, 0.f, m_flShadowAlpha };
    ImColor Color_AimFOV{ 1.f, 1.f, 1.f, 0.35f };
    ImColor Color_Crosshair{ 0.f, 1.f, 0.f, 1.f };
};

class ConfigManager
{
private:
    std::string m_szConfigPath;
    json GetDefaultConfig();
public:
    // AppData\Local 内に作るフォルダの名前
    bool InitConfigSystem(const std::string configDirName);
    bool InitConfigSystem(const std::string baseDirName, const std::string configDirName);
    bool CreateNewConfig(const std::string& configName);
    void DeleteConfig(const std::string& configName);
    std::vector<std::string> GetFileList(); // ConfigDir内の設定ファイル一覧を取得
    void LoadSetting(const std::string filename);
    void SaveSetting(const std::string filename);
};

extern std::atomic<bool> g_ApplicationActive;
extern GlobalVars g;
extern ConfigManager config;
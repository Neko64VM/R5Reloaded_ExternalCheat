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
using json = nlohmann::json;

// Global vars
struct GlobalVars
{
    // System
    bool bShowMenu{ false };

    // KeyBoard Hook/Input
    std::string newConfigName{};
    bool GenerateFlag{ false };

    // Window Data
    HWND hGameWindow{};
    RECT rcSize{};
    POINT ptPoint{};

    // KeyData
    DWORD dwMenuKey{ VK_INSERT };
    DWORD dwAimKey0{ VK_RBUTTON };
    DWORD dwAimKey1{ VK_LBUTTON };

    // AimBot
    bool AimBotEnable{ false };
    bool bShowFOV{ true };
    bool bRainbowFOV{ false };
    int AimFOV{ 40 };
    float AimSmooth{ 12.5f };
    int AimMode{ 0 };
    int AimKeyMode{ 2 }; // None, and, or
    int AimTargetBone{ 1 };
    int AimMaxDistance{ 25 };

    // Visual
    bool VisualEnable{ true };
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
    int ESP_BoxType{ 1 };
    int ESP_BoxRenderMode{ 1 };
    int ESP_MaxDistance{ 500 };

    // Misc
    bool SpectatorListEnable{ true };
    bool CrosshairEnable{ false };
    int CrosshairSize{ 5 };
    int CrosshairType{ 0 };
    int MaxFramerate{ 185 };
    bool RecoilControllSystem{ false };
    float RCS_Scale{ 1.f };
    float VMG_Rate{ 5.f };
    int VMG_Type{ 0 };

    // Colors
    float m_flGlobalAlpha{ 0.925f };
    float m_flShadowAlpha{ 0.235f };
    ImColor Color_ESP_Team{ 0.f, 0.75f, 1.f, m_flGlobalAlpha };
    ImColor Color_ESP_Enemy{ 1.f, 1.f, 1.f, m_flGlobalAlpha };
    ImColor Color_ESP_Visible{ 1.f, 0.5f, 0.f, m_flGlobalAlpha };
    ImColor Color_ESP_AimTarget{ 1.f, 0.f, 0.f, m_flShadowAlpha };
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
    // AppData\Local ���ɍ��t�H���_�̖��O
    bool InitConfigSystem(const std::string configDirName);
    bool InitConfigSystem(const std::string baseDirName, const std::string configDirName);
    bool CreateNewConfig(const std::string& configName);
    void DeleteConfig(const std::string& configName);
    std::vector<std::string> GetFileList(); // ConfigDir���̐ݒ�t�@�C���ꗗ���擾
    void LoadSetting(const std::string filename);
    void SaveSetting(const std::string filename);
};

extern std::atomic<bool> g_ApplicationActive;
extern GlobalVars g;
extern ConfigManager config;
#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Config.h"
#include "ConfigManager.h"

namespace
{
    std::string AddJsonExtension(const std::string& configName)
    {
        std::filesystem::path path(configName);
        if (path.extension() != ".json")
            path += ".json";

        return path.string();
    }

    std::filesystem::path MakeConfigPath(const std::string& configDir, const std::string& configName)
    {
        return std::filesystem::path(configDir) / AddJsonExtension(configName);
    }

    void ApplyConfigJson(const json& JSON)
    {
        g.dwMenuKey = JSON.at("dwMenuKey").get<int>();
        g.dwAimKey0 = JSON.at("dwAimKey0").get<int>();
        g.dwAimKey1 = JSON.at("dwAimKey1").get<int>();

        // Aim
        g.AimBotEnable = JSON.at("bAimEnable").get<bool>();
		g.bVisCheck = JSON.at("bVisCheck").get<bool>();
		g.bRemoveSway = JSON.at("bRemoveSway").get<bool>();
        g.bAimAtTeam = JSON.at("bAimAtTeam").get<bool>();
        g.bShowFOV = JSON.at("bShowFOV").get<bool>();
        g.bRainbowFOV = JSON.at("bRainbowFOV").get<bool>();
        g.AimFOV = JSON.at("AimFOV").get<int>();
        g.AimSmooth = JSON.at("AimSmooth").get<float>();
        g.AimMode = JSON.at("AimMode").get<int>();
        g.AimKeyMode = JSON.at("AimKeyMode").get<int>();
        g.AimTargetBone = JSON.at("AimTargetBone").get<int>();
        g.AimMaxDistance = JSON.at("AimMaxDistance").get<int>();

        // Visual
        g.VisualEnable = JSON.at("bVisualEnable").get<bool>();
		g.RadarEnable = JSON.at("bRadarEnable").get<bool>();
        g.ESP_Team = JSON.at("bVTeam").get<bool>();
        g.ESP_NPC = JSON.at("bVNPC").get<bool>();
        g.bBox = JSON.at("bBox").get<bool>();
        g.bFilled = JSON.at("bFilled").get<bool>();
        g.bLine = JSON.at("bLine").get<bool>();
        g.bSkeleton = JSON.at("bSkeleton").get<bool>();
        g.bHealth = JSON.at("bHealth").get<bool>();
        g.bDistance = JSON.at("bDistance").get<bool>();
        g.bName = JSON.at("bName").get<bool>();
        g.bWeapon = JSON.at("bWeapon").get<bool>();
        g.GlowStyle = JSON.at("GlowStyle").get<int>();
        g.RadarScale= JSON.at("fRadarScale").get<float>();
        g.ESP_BoxType = JSON.at("BoxType").get<int>();
        g.ESP_BoxRenderMode = JSON.at("BoxMode").get<int>();
        g.ESP_MaxDistance = JSON.at("iVDistance").get<int>();

        // Misc
        g.MaxFramerate = JSON.at("MaxFramerate").get<int>();
        g.SpectatorListEnable = JSON.at("bSpectatorList").get<bool>();
        g.CrosshairEnable = JSON.at("bCrosshair").get<bool>();
        g.CrosshairSize = JSON.at("ChairSize").get<int>();
        g.CrosshairType = JSON.at("ChairType").get<int>();
        g.AimAssistMod = JSON.at("AimAssist").get<int>();
        g.RecoilControllSystem = JSON.at("bRCS").get<bool>();
        g.RCS_Scale = JSON.at("fRCSScale").get<float>();
        g.VMG_Type = JSON.at("ViewModelMode").get<int>();
        g.VMG_Rate = JSON.at("ViewModelRGB").get<int>();
        g.bSkinChanger = JSON.at("bSkinChanger").get<int>();

        // Color
        auto color = JSON.at("C_Team");

        g.Color_ESP_Team = {
            color.at("r").get<float>(),
            color.at("g").get<float>(),
            color.at("b").get<float>(),
            color.at("a").get<float>()
        };

        color = JSON.at("C_Enemy");

        g.Color_ESP_Enemy = {
           color.at("r").get<float>(),
           color.at("g").get<float>(),
           color.at("b").get<float>(),
           color.at("a").get<float>()
        };

        color = JSON.at("C_Visible");

        g.Color_ESP_Visible = {
           color.at("r").get<float>(),
           color.at("g").get<float>(),
           color.at("b").get<float>(),
           color.at("a").get<float>()
        };

        color = JSON.at("C_Target");

        g.Color_ESP_AimTarget = {
           color.at("r").get<float>(),
           color.at("g").get<float>(),
           color.at("b").get<float>(),
           color.at("a").get<float>()
        };

        color = JSON.at("C_AFOV");

        g.Color_AimFOV = {
           color.at("r").get<float>(),
           color.at("g").get<float>(),
           color.at("b").get<float>(),
           color.at("a").get<float>()
        };

        color = JSON.at("C_Crosshair");

        g.Color_Crosshair = {
           color.at("r").get<float>(),
           color.at("g").get<float>(),
           color.at("b").get<float>(),
           color.at("a").get<float>()
        };
    }
}

std::string ConfigManager::GetAppDataPath(const GUID& id)
{
    PWSTR path = nullptr;
    std::string result;

    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path)))
        result = ConvertWideToMultiByte(path);

    CoTaskMemFree(path);
    return result;
}

std::string ConfigManager::ConvertWideToMultiByte(const std::wstring& target_str)
{
    if (target_str.empty())
        return std::string();

    const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize == 0)
        return std::string();

    std::string vOut(bufferSize, '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, vOut.data(), bufferSize, nullptr, nullptr) == 0)
        return std::string();

    vOut.pop_back();
    return vOut;
}

std::vector<std::string> ConfigManager::GetConfigList()
{
    std::vector<std::string> configList;

    if (!std::filesystem::exists(m_szConfigPath))
        return configList;

    for (const auto& entry : std::filesystem::directory_iterator(m_szConfigPath))
    {
        if (!entry.is_regular_file())
            continue;

        auto path = entry.path();
        if (path.extension() == ".json")
            configList.push_back(path.filename().string());
    }

    return configList;
}

bool ConfigManager::CreateConfig(const std::string& configName)
{
    std::filesystem::path newConfig = MakeConfigPath(m_szConfigPath, configName);

    if (std::filesystem::exists(newConfig))
        return false;

    std::ofstream out(newConfig);
    if (!out.good())
        return false;

    out << GetDefaultConfig().dump(4);
    return true;
}

void ConfigManager::DeleteConfig(const std::string& configName)
{
    std::filesystem::path delConfig = MakeConfigPath(m_szConfigPath, configName);

    if (std::filesystem::exists(delConfig))
        std::filesystem::remove(delConfig);
}

bool ConfigManager::InitConfigSystem(const std::string& configDirName)
{
    m_szConfigPath = (std::filesystem::path(GetAppDataPath(FOLDERID_LocalAppData)) / configDirName).string();

    if (m_szConfigPath.find(":\\") == std::string::npos)
        return false;

    std::filesystem::create_directories(m_szConfigPath);
    CreateConfig(m_szDefaultConfName);
    LoadSetting(m_szDefaultConfName);

    return true;
}

bool ConfigManager::InitConfigSystem(const std::string& baseDirName, const std::string& configDirName)
{
    std::filesystem::path baseDir = std::filesystem::path(GetAppDataPath(FOLDERID_LocalAppData)) / baseDirName;
    m_szConfigPath = (baseDir / configDirName).string();

    if (m_szConfigPath.find(":\\") == std::string::npos)
        return false;

    std::filesystem::create_directories(m_szConfigPath);
    CreateConfig(m_szDefaultConfName);
    LoadSetting(m_szDefaultConfName);

    return true;
}

json ConfigManager::GetDefaultConfig()
{
    return json{
        { "dwMenuKey", VK_INSERT },
        { "dwAimKey0" , VK_RBUTTON },
        { "dwAimKey1", VK_LBUTTON },

        // Aim
        { "bAimEnable", true },
        { "bVisCheck", true },
        { "bRemoveSway", true },
        { "bAimAtTeam", false },
        { "bShowFOV", true },
        { "bRainbowFOV", false },
        { "AimFOV", 150 },
        { "AimSmooth", 2.5f },
        { "AimMode", 0 },
        { "AimKeyMode", 2 },
        { "AimTargetBone", 1 },
        { "AimMaxDistance", 100 },

        // Visual
        { "bVisualEnable", true },
        { "bRadarEnable", true },
        { "bVTeam", true },
        { "bVNPC", true },
        { "bBox", true },
        { "bLine", false },
        { "bFilled", false },
        { "bSkeleton", true },
        { "bHealth", true },
        { "bDistance", true },
        { "bName", true },
        { "bWeapon", true },
        { "GlowStyle", 0 },
        { "fRadarScale", 12.f },
        { "BoxType", 1 },
        { "BoxMode", 1 },
        { "iVDistance", 500 },

        // Misc
        { "MaxFramerate", 165 },
        { "bSpectatorList", true },
        { "bCrosshair", false },
        { "ChairSize", 5 },
        { "ChairType", 0 },
        { "AimAssist", 0.6f },
        { "bRCS", false },
        { "fRCSScale", 1.f },
        { "ViewModelMode", 0 },
        { "ViewModelRGB", 5.f },
        { "bSkinChanger", false },

        { "C_Enemy", {
            { "r", 1.f },
            { "g", 0.f },
            { "b", 0.f },
            { "a", 1.f }}},
        { "C_Visible", {
            { "r", 0.f },
            { "g", 1.f },
            { "b", 0.f },
            { "a", 1.f }}},
        { "C_Team", {
            { "r", 0.f },
            { "g", 1.f },
            { "b", 1.f },
            { "a", 1.f }}},
        { "C_Target", {
            { "r", 1.f },
            { "g", 1.f },
            { "b", 1.f },
            { "a", 1.f }}},
        { "C_AFOV", {
            { "r", 1.f },
            { "g", 1.f },
            { "b", 1.f },
            { "a", 0.75f }}},
        { "C_Crosshair", {
            { "r", 0.f },
            { "g", 1.f },
            { "b", 0.f },
            { "a", 1.f }}}
    };
}

void ConfigManager::LoadSetting(const std::string& configName)
{
    std::filesystem::path configPath = MakeConfigPath(m_szConfigPath, configName);
    std::ifstream file(configPath);

    if (!file.good())
        return;

    try
    {
        json JSON;
        file >> JSON;

        ApplyConfigJson(JSON);
    }
    catch (const std::exception&)
    {
        file.close();
        DeleteConfig(configName);
        CreateConfig(configName);
        ApplyConfigJson(GetDefaultConfig());
    }
}

void ConfigManager::SaveSetting(const std::string& configName)
{
    std::filesystem::path configPath = MakeConfigPath(m_szConfigPath, configName);
    json JSON = GetDefaultConfig();

    try
    {
        std::ifstream file(configPath);
        if (file.good())
            file >> JSON;
    }
    catch (const std::exception&)
    {
        JSON = GetDefaultConfig();
    }

    JSON["dwMenuKey"] = g.dwMenuKey;
    JSON["dwAimKey0"] = g.dwAimKey0;
    JSON["dwAimKey1"] = g.dwAimKey1;

        // Aim
    JSON["bAimEnable"] = g.AimBotEnable;
    JSON["bVisCheck"] = g.bVisCheck;
    JSON["bRemoveSway"] = g.bRemoveSway;
    JSON["bAimAtTeam"] = g.bRemoveSway;
    JSON["bShowFOV"] = g.bShowFOV;
    JSON["bRainbowFOV"] = g.bRainbowFOV;
    JSON["AimFOV"] = g.AimFOV;
    JSON["AimSmooth"] = g.AimSmooth;
    JSON["AimMode"] = g.AimMode;
    JSON["AimKeyMode"] = g.AimKeyMode;
    JSON["AimTargetBone"] = g.AimTargetBone;
    JSON["AimMaxDistance"] = g.AimMaxDistance;

        // Visual
    JSON["bVisualEnable"] = g.VisualEnable;
    JSON["bRadarEnable"] = g.RadarEnable;
    JSON["bVTeam"] = g.ESP_Team;
    JSON["bVNPC"] = g.ESP_NPC;
    
    JSON["bBox"] = g.bBox;
    JSON["bFilled"] = g.bFilled;
    JSON["bLine"] = g.bLine;
    JSON["bSkeleton"] = g.bSkeleton;
    JSON["bHealth"] = g.bHealth;
    JSON["bDistance"] = g.bDistance;
    JSON["bName"] = g.bName;
    JSON["bWeapon"] = g.bWeapon;
    JSON["GlowStyle"] = g.GlowStyle;
    JSON["fRadarScale"] = g.RadarScale;
    JSON["BoxType"] = g.ESP_BoxType;
    JSON["BoxMode"] = g.ESP_BoxRenderMode;
    JSON["iVDistance"] = g.ESP_MaxDistance;

    // Misc
    JSON["MaxFramerate"] = g.MaxFramerate;
    JSON["bSpectatorList"] = g.SpectatorListEnable;
    JSON["bCrosshair"] = g.CrosshairEnable;
    JSON["ChairSize"] = g.CrosshairSize;
    JSON["ChairType"] = g.CrosshairType;
    
    JSON["AimAssist"] = g.AimAssistMod;
    JSON["bRCS"] = g.RecoilControllSystem;
    JSON["fRCSScale"] = g.RCS_Scale;
    JSON["ViewModelMode"] = g.VMG_Rate;
    JSON["ViewModelRGB"] = g.VMG_Type;
    JSON["bSkinChanger"] = g.bSkinChanger;

    // Color
    JSON["C_Team"] = {
        { "r", g.Color_ESP_Team.Value.x },
        { "g", g.Color_ESP_Team.Value.y },
        { "b", g.Color_ESP_Team.Value.z },
        { "a", g.Color_ESP_Team.Value.w }
    };

    JSON["C_Enemy"] = {
        { "r", g.Color_ESP_Enemy.Value.x },
        { "g", g.Color_ESP_Enemy.Value.y },
        { "b", g.Color_ESP_Enemy.Value.z },
        { "a", g.Color_ESP_Enemy.Value.w }
    };

    JSON["C_Visible"] = {
        { "r", g.Color_ESP_Visible.Value.x },
        { "g", g.Color_ESP_Visible.Value.y },
        { "b", g.Color_ESP_Visible.Value.z },
        { "a", g.Color_ESP_Visible.Value.w }
    };

    JSON["C_Target"] = {
        { "r", g.Color_ESP_AimTarget.Value.x },
        { "g", g.Color_ESP_AimTarget.Value.y },
        { "b", g.Color_ESP_AimTarget.Value.z },
        { "a", g.Color_ESP_AimTarget.Value.w }
    };

    JSON["C_AFOV"] = {
        { "r", g.Color_AimFOV.Value.x },
        { "g", g.Color_AimFOV.Value.y },
        { "b", g.Color_AimFOV.Value.z },
        { "a", g.Color_AimFOV.Value.w }
    };

    JSON["C_Crosshair"] = {
        { "r", g.Color_Crosshair.Value.x },
        { "g", g.Color_Crosshair.Value.y },
        { "b", g.Color_Crosshair.Value.z },
        { "a", g.Color_Crosshair.Value.w }
    };

    std::ofstream outputFile(configPath, std::ios::trunc);
    if (outputFile.good())
    {
        outputFile << JSON.dump(4);
        std::cout << "[ LOG ] json updated - ConfigManager::SaveSetting()" << std::endl;
    }
    else
    {
        std::cout << "[ LOG ] failed to update json - ConfigManager::SaveSetting()" << std::endl;
    }
}

ConfigManager config;

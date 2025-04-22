#include <fstream>
#include <shlobj.h>
#include "Config.h"

GlobalVars g;
ConfigManager config;
std::atomic<bool> g_ApplicationActive(true);

std::vector<std::string> ConfigManager::GetFileList()
{
    std::vector<std::string> configList;

    for (const auto& entry : std::filesystem::directory_iterator(m_szConfigPath))
    {
        if (entry.is_regular_file())
        {
            auto path = entry.path();
            if (path.extension() == ".json")
                configList.push_back(path.filename().string());
        }
    }

    return configList;
}

bool ConfigManager::CreateNewConfig(const std::string& configName)
{
    std::string newConfig = m_szConfigPath + "\\" + configName + ".json";

    // json���Ȃ��ꍇ�̓t�@�C�����쐬
    if (!utils::file::IsExistsFile(newConfig))
    {
        // �t�@�C�����쐬
        std::ofstream fFile(newConfig);
        fFile.close();

        // json�ɏ�������
        std::ifstream in(newConfig);
        if (!in || in.peek() == std::ifstream::traits_type::eof())
        {
            std::ofstream out(newConfig);
            out << this->GetDefaultConfig().dump(4); // �C���f���g (4)

            return true;
        }
    }

    return false;
}

void ConfigManager::DeleteConfig(const std::string& configName)
{
    std::string delConfig = m_szConfigPath + "\\" + configName;

    if (utils::file::IsExistsFile(delConfig)) {
        std::filesystem::remove(delConfig);
    }
}

bool ConfigManager::InitConfigSystem(const std::string configDirName)
{
    // �e��Path���擾 (AppData)
    m_szConfigPath = utils::file::GetAppDataPath(FOLDERID_LocalAppData) + "\\" + configDirName;

    // �h���C�u�̃��[�g�̕����񂪊܂܂�Ă��Ȃ������� / Config�p�̃f�B���N�g�������݂��Ȃ�������쐬
    if (m_szConfigPath.find(":\\") == std::string::npos)
        return false;
    else if (!utils::file::IsExistsDirectory(m_szConfigPath))
        std::filesystem::create_directory(m_szConfigPath);

    std::string defaultConfig = m_szConfigPath + "\\" + "Default.json";

    // json���Ȃ��ꍇ�̓t�@�C�����쐬
    if (!utils::file::IsExistsFile(defaultConfig))
    {
        // �t�@�C�����쐬
        std::ofstream fFile(defaultConfig);
        fFile.close();

        // json�ɏ�������
        std::ifstream in(defaultConfig);
        if (!in || in.peek() == std::ifstream::traits_type::eof())
        {
            std::ofstream out(defaultConfig);
            out << this->GetDefaultConfig().dump(4); // �C���f���g (4)
        }
    }

    // config.json����ݒ�����[�h
    config.LoadSetting("Default.json");
}

bool ConfigManager::InitConfigSystem(const std::string baseDirName, const std::string configDirName)
{
    // �e��Path���擾 (AppData)
    std::string baseDir = utils::file::GetAppDataPath(FOLDERID_LocalAppData) + "\\" + baseDirName;
    m_szConfigPath = baseDir + "\\" + configDirName;

    if (!utils::file::IsExistsDirectory(baseDir))
        std::filesystem::create_directory(baseDir);
    if (!utils::file::IsExistsDirectory(m_szConfigPath))
        std::filesystem::create_directory(m_szConfigPath);

    // �h���C�u�̃��[�g�̕����񂪊܂܂�Ă��Ȃ������� / Config�p�̃f�B���N�g�������݂��Ȃ�������쐬
    if (m_szConfigPath.find(":\\") == std::string::npos)
        return false;
    else if (!utils::file::IsExistsDirectory(m_szConfigPath))
        std::filesystem::create_directory(m_szConfigPath);

    std::string defaultConfig = m_szConfigPath + "\\" + "Default.json";

    // json���Ȃ��ꍇ�̓t�@�C�����쐬
    if (!utils::file::IsExistsFile(defaultConfig))
    {
        // �t�@�C�����쐬
        std::ofstream fFile(defaultConfig);
        fFile.close();

        // json�ɏ�������
        std::ifstream in(defaultConfig);
        if (!in || in.peek() == std::ifstream::traits_type::eof())
        {
            std::ofstream out(defaultConfig);
            out << this->GetDefaultConfig().dump(4); // �C���f���g (4)
        }
    }

    // config.json����ݒ�����[�h
    config.LoadSetting("Default.json");
}

// Default Config
json ConfigManager::GetDefaultConfig()
{
    return json{
        // Keys
        { "MenuKey", VK_INSERT },
        { "AimKey0", VK_RBUTTON },
        { "AimKey1", VK_LBUTTON },
        // Aim
        { "AimBotEnable", false },
        { "bShowFOV", true },
        { "bRainbowFOV", false },
        { "AimFOV", 40 },
        { "AimSmooth", 12.5f },
        { "AimMode", 0 },
        { "AimKeyMode", 2 },
        { "AimTargetBone", 1 },
        { "AimMaxDistance", 25 },
        // Visual
        { "VisualEnable", true },
        { "GlowStyle", 0 },
        { "ESP_NPC", true },
        { "ESP_Team", false },
        { "bBox", true },
        { "bFilled", false },
        { "bLine", false },
        { "bSkeleton", true },
        { "bHealth", true },
        { "bDistance", true },
        { "bName", true },
        { "bWeapon", true },
        { "ESP_BoxType", 1 },
        { "ESP_BoxRenderMode", 1 },
        { "ESP_MaxDistance", 500 },
        // Misc
        { "SpectatorListEnable", true },
        { "CrosshairEnable", false },
        { "CrosshairSize", 5 },
        { "CrosshairType", 0 },
        { "MaxFramerate", 165 },

        { "RecoilControllSystem", false },
        { "RCS_Scale", 1.f },

        { "VMG_Rate", 5.f },
        { "VMG_Type", 0 },
        // Color
        { "GlobalAlpha", 0.925f },
        { "ShadowAlpha", 0.235f },
        { "Col_ESP_Team", {
            {"r", 0.f },
            {"g", 0.75f },
            {"b", 1.f },
            {"a", 0.925f }}},
        { "Col_ESP_Enemy", {
            {"r", 1.f },
            {"g", 1.f },
            {"b", 1.f },
            {"a", 0.925f }}},
        { "Color_ESP_Visible", {
            {"r", 1.f },
            {"g", 0.5f },
            {"b", 0.f },
            {"a", 0.925f }}},
        { "Col_ESP_AimTarget", {
            {"r", 1.f },
            {"g", 0.f },
            {"b", 0.f },
            {"a", 0.925f }}},
        { "Col_ESP_Shadow", {
            {"r", 0.f },
            {"g", 0.f },
            {"b", 0.f },
            {"a", 0.235f }}},

        { "Col_AimFOV", {
            {"r", 1.f },
            {"g", 1.f },
            {"b", 1.f },
            {"a", 0.35f }}},
        { "Col_Crosshair", {
            {"r", 0.f },
            {"g", 1.f },
            {"b", 0.f },
            {"a", 1.f }}},
    };
}

void ConfigManager::LoadSetting(const std::string filename)
{
    std::string configPath = m_szConfigPath + "\\" + filename;
    std::ifstream file(configPath);

    if (file.good()) {
        json JSON;
        file >> JSON;

        try
        {
            // Keys
            g.dwMenuKey = JSON["MenuKey"];
            g.dwAimKey0 = JSON["AimKey0"];
            g.dwAimKey1 = JSON["AimKey1"];
            // Aim
            g.AimBotEnable = JSON["AimBotEnable"];
            g.bShowFOV = JSON["bShowFOV"];
            g.bRainbowFOV = JSON["bRainbowFOV"];
            g.AimFOV = JSON["AimFOV"];
            g.AimSmooth = JSON["AimSmooth"];
            g.AimMode = JSON["AimMode"];
            g.AimKeyMode = JSON["AimKeyMode"];
            g.AimTargetBone = JSON["AimTargetBone"];
            g.AimMaxDistance = JSON["AimMaxDistance"];
            // Visual
            g.VisualEnable = JSON["VisualEnable"];
            g.GlowStyle = JSON["GlowStyle"];
            g.ESP_NPC = JSON["ESP_NPC"];
            g.ESP_Team = JSON["ESP_Team"];
            g.bBox = JSON["bBox"];
            g.bFilled = JSON["bFilled"];
            g.bLine = JSON["bLine"];
            g.bSkeleton = JSON["bSkeleton"];
            g.bHealth = JSON["bHealth"];
            g.bDistance = JSON["bDistance"];
            g.bName = JSON["bName"];
            g.bWeapon = JSON["bWeapon"];
            g.ESP_BoxType = JSON["ESP_BoxType"];
            g.ESP_BoxRenderMode = JSON["ESP_BoxRenderMode"];
            g.ESP_MaxDistance = JSON["ESP_MaxDistance"];
            // Misc
            g.SpectatorListEnable = JSON["SpectatorListEnable"];
            g.CrosshairEnable = JSON["CrosshairEnable"];
            g.CrosshairSize = JSON["CrosshairSize"];
            g.CrosshairType = JSON["CrosshairType"];
            g.MaxFramerate = JSON["MaxFramerate"];
            g.RecoilControllSystem = JSON["RecoilControllSystem"]; // RCS
            g.RCS_Scale = JSON["RCS_Scale"];
            g.VMG_Rate = JSON["VMG_Rate"];
            g.VMG_Type = JSON["VMG_Type"];
            // Color
            g.m_flGlobalAlpha = JSON["GlobalAlpha"];
            g.m_flShadowAlpha = JSON["ShadowAlpha"];

            auto col_team = JSON["Col_ESP_Team"];
            auto col_enemy = JSON["Col_ESP_Enemy"];
            auto col_visible = JSON["Color_ESP_Visible"];
            auto col_target = JSON["Col_ESP_AimTarget"];
            auto col_shadow = JSON["Col_ESP_Shadow"];
            auto col_fov = JSON["Col_AimFOV"];
            auto col_crosshair = JSON["Col_Crosshair"];

            g.Color_ESP_Team = ImVec4(col_team["r"], col_team["g"], col_team["b"], col_team["a"]);
            g.Color_ESP_Enemy = ImVec4(col_enemy["r"], col_enemy["g"], col_enemy["b"], col_enemy["a"]);
            g.Color_ESP_Visible = ImVec4(col_visible["r"], col_visible["g"], col_visible["b"], col_visible["a"]);
            g.Color_ESP_AimTarget = ImVec4(col_target["r"], col_target["g"], col_target["b"], col_target["a"]);
            g.Color_ESP_Shadow = ImVec4(col_shadow["r"], col_shadow["g"], col_shadow["b"], col_shadow["a"]);
            g.Color_AimFOV = ImVec4(col_fov["r"], col_fov["g"], col_fov["b"], col_fov["a"]);
            g.Color_Crosshair = ImVec4(col_crosshair["r"], col_crosshair["g"], col_crosshair["b"], col_crosshair["a"]);
        }
        catch (const std::exception&)
        {
            file.close();
            DeleteConfig(filename);
            CreateNewConfig(filename);
        }
    }

    file.close();
}


void ConfigManager::SaveSetting(const std::string filename)
{
    std::string configPath = m_szConfigPath + "\\" + filename;
    std::ifstream file(configPath);

    if (file.good()) {
        json JSON;
        file >> JSON;

        // Keys
        JSON["MenuKey"] = g.dwMenuKey;
        JSON["AimKey0"] = g.dwAimKey0;
        JSON["AimKey1"] = g.dwAimKey1;
        // Aim
        JSON["AimBotEnable"] = g.AimBotEnable;
        JSON["bShowFOV"] = g.bShowFOV;
        JSON["bRainbowFOV"] = g.bRainbowFOV;
        JSON["AimFOV"] = g.AimFOV;
        JSON["AimSmooth"] = g.AimSmooth;
        JSON["AimMode"] = g.AimMode;
        JSON["AimKeyMode"] = g.AimKeyMode;
        JSON["AimTargetBone"] = g.AimTargetBone;
        JSON["AimMaxDistance"] = g.AimMaxDistance;
        // Visual
        JSON["VisualEnable"] = g.VisualEnable;
        JSON["GlowStyle"] = g.GlowStyle;
        JSON["ESP_NPC"] = g.ESP_NPC;
        JSON["ESP_Team"] = g.ESP_Team;
        JSON["bBox"] = g.bBox;
        JSON["bFilled"] = g.bFilled;
        JSON["bLine"] = g.bLine;
        JSON["bSkeleton"] = g.bSkeleton;
        JSON["bHealth"] = g.bHealth;
        JSON["bDistance"] = g.bDistance;
        JSON["bName"] = g.bName;
        JSON["bWeapon"] = g.bWeapon;
        JSON["ESP_BoxType"] = g.ESP_BoxType;
        JSON["ESP_BoxRenderMode"] = g.ESP_BoxRenderMode;
        JSON["ESP_MaxDistance"] = g.ESP_MaxDistance;
        // Misc
        JSON["SpectatorListEnable"] = g.SpectatorListEnable;
        JSON["CrosshairEnable"] = g.CrosshairEnable;
        JSON["CrosshairSize"] = g.CrosshairSize;
        JSON["CrosshairType"] = g.CrosshairType;
        JSON["MaxFramerate"] = g.MaxFramerate;

        JSON["VMG_Rate"] = g.VMG_Rate;
        JSON["VMG_Type"] = g.VMG_Type;

        JSON["RecoilControllSystem"] = g.RecoilControllSystem;
        JSON["RCS_Scale"] = g.RCS_Scale;

        // Color
        JSON["GlobalAlpha"] = g.m_flGlobalAlpha;
        JSON["ShadowAlpha"] = g.m_flShadowAlpha;
        
        JSON["Col_ESP_Team"] = {
            { "r", g.Color_ESP_Team.Value.x },
            { "g", g.Color_ESP_Team.Value.y },
            { "b", g.Color_ESP_Team.Value.z },
            { "a", g.Color_ESP_Team.Value.w }
        };
        JSON["Col_ESP_Enemy"] = {
            { "r", g.Color_ESP_Enemy.Value.x },
            { "g", g.Color_ESP_Enemy.Value.y },
            { "b", g.Color_ESP_Enemy.Value.z },
            { "a", g.Color_ESP_Enemy.Value.w }
        };
        JSON["Col_ESP_Visible"] = {
           { "r", g.Color_ESP_Visible.Value.x },
           { "g", g.Color_ESP_Visible.Value.y },
           { "b", g.Color_ESP_Visible.Value.z },
           { "a", g.Color_ESP_Visible.Value.w }
        };
        JSON["Col_ESP_AimTarget"] = {
            { "r", g.Color_ESP_AimTarget.Value.x },
            { "g", g.Color_ESP_AimTarget.Value.y },
            { "b", g.Color_ESP_AimTarget.Value.z },
            { "a", g.Color_ESP_AimTarget.Value.w }
        };
        JSON["Col_ESP_Shadow"] = {
            { "r", g.Color_ESP_Shadow.Value.x },
            { "g", g.Color_ESP_Shadow.Value.y },
            { "b", g.Color_ESP_Shadow.Value.z },
            { "a", g.Color_ESP_Shadow.Value.w }
        };
        JSON["Col_AimFOV"] = {
            { "r", g.Color_AimFOV.Value.x },
            { "g", g.Color_AimFOV.Value.y },
            { "b", g.Color_AimFOV.Value.z },
            { "a", g.Color_AimFOV.Value.w }
        };
        JSON["Col_Crosshair"] = {
            { "r", g.Color_Crosshair.Value.x },
            { "g", g.Color_Crosshair.Value.y },
            { "b", g.Color_Crosshair.Value.z },
            { "a", g.Color_Crosshair.Value.w }
        };

        std::ofstream outputFile(configPath, std::ios::trunc);

        if (outputFile.good()) {
            outputFile << JSON.dump(4);
            std::cout << "[ LOG ] json updated - ConfigManager::SaveSetting()" << std::endl;
        }
        else {
            std::cout << "[ LOG ] failed to update json - ConfigManager::SaveSetting()" << std::endl;
        }
    }

    file.close();
}
#include "CFramework.h"
#include "../Framework/Config/ConfigManager.h"
#include "../Framework/Input/Input.hpp"

// Menu, Config
int Index = 0;
int BindingID = 0;
int FileNum = 0;
bool deleteFlag = false;
const char* AimBoneList[]{ "Head", "Chest"};
const char* AimKeyModeList[]{ "None Key", "and", "or" };
const char* BoxRenderModeList[]{ "Bone", "BoundingBox" };
const char* BoxTypeList[]{ "Simple", "Cornered" };
const char* CrosshairList[]{ "Cross", "Circle" };
const char* GlowStyleList[]{ "None", "Simple" };
const char* ViewModelGlowTypeList[]{ "None", "Border Only", "Border + Filled"};
const std::vector<const char*> MenuStringList{ "AimBot", "Visual", "Misc", "Setting" };
const std::vector<const char*> MenuIconList{ ICON_FA_CROSSHAIRS, ICON_FA_EYE, ICON_FA_BARS, ICON_FA_GEAR };

OverlayInput input;

void CFramework::RenderMenu()
{
    Custom::PushTheme(theme);

    ImGui::SetNextWindowSize(ImVec2(900.f, 600.f), ImGuiCond_Always);

    ImGui::Begin("##menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    Custom::BeginSideNav("CUSTOM MENU", 170.f, theme);
    {
        Custom::SideNavGroup("HACK", theme);
        Custom::SideNavItem("AimBot", 0, &Index, theme, ICON_FA_CROSSHAIRS);
        Custom::SideNavItem("Visual", 1, &Index, theme, ICON_FA_EYE);
        Custom::SideNavItem("Exploit", 2, &Index, theme, ICON_FA_BARS);

        Custom::SideNavGroup("SYSTEM", theme);
        Custom::SideNavItem("Config", 3, &Index, theme, ICON_FA_FLOPPY_DISK);
        Custom::SideNavItem("Setting", 4, &Index, theme, ICON_FA_GEAR);
    }
    Custom::EndSideNav();

    ImGui::SameLine(0.f, 12.f);

    Custom::BeginContentArea(&Index, theme);

    // Page 1
    if (Custom::BeginTwoColumnPage(0, theme))
    {
        Custom::BeginSection("AimBot", theme);
        /*--------------------------------------------------------*/
		Custom::Toggle("AimBot", &g.AimBotEnable, theme);
		Custom::Checkbox("VisCheck", &g.bVisCheck, theme);
        Custom::Checkbox("RemoveSway", &g.bRemoveSway, theme);

        ImGui::Dummy(ImVec2(0.f, 8.f));

		Custom::SectionHeader("AimBot Setting", theme);
		Custom::SliderInt("AimFOV", &g.AimFOV, 100, 500, theme);
		Custom::SliderFloat("Smooth", &g.AimSmooth, 1.f, 30.f, theme);
        Custom::SliderInt("AimDistance", &g.AimMaxDistance, 15, 500, theme);

		Custom::Combo("TargetBone", &g.AimTargetBone, AimBoneList, IM_ARRAYSIZE(AimBoneList), theme);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);

        Custom::TwoColumnSplit(theme);

        Custom::BeginSection("AimBot Config", theme);
        /*--------------------------------------------------------*/

        Custom::Checkbox("Draw FOV", &g.bShowFOV, theme);
        Custom::Checkbox("Rainbow FOV", &g.bRainbowFOV, theme);
        Custom::SliderInt("Aim FOV", &g.AimFOV, 100, 500, theme);
        ImGui::ColorEdit3("Color", &g.Color_AimFOV.Value.x);

        // 重複防止
        if (g.dwAimKey0 == g.dwAimKey1)
            g.dwAimKey1 = NULL;
        /*--------------------------------------------------------*/
        ImGui::Dummy(ImVec2(0.f, 8.f));
        Custom::SectionHeader("KeyBinder", theme);
        /*--------------------------------------------------------*/

        Custom::Combo("AimKeyMode", &g.AimKeyMode, AimKeyModeList, IM_ARRAYSIZE(AimKeyModeList), theme);

		static bool bFirstKeyBinding = false;
        static bool bSecondKeyBinding = false;
        static DWORD key01{ NULL };
        static DWORD key02{ NULL };
        const char* keyName = bFirstKeyBinding ? "<Press any key...>" : OverlayInput::GetKeyName(key01);

        if (ImGui::Button(keyName, ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
        {
			bFirstKeyBinding = true;
            input.KeyBind(key01, bFirstKeyBinding);
        }

        if (bFirstKeyBinding)
            input.KeyBind(key01, bFirstKeyBinding);

        const char* keyName1 = bSecondKeyBinding ? "<Press any key...>" : OverlayInput::GetKeyName(key02);

        ImGui::PushID("SecondKeyBind");
        if (ImGui::Button(keyName1, ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
        {
            bSecondKeyBinding = true;
            input.KeyBind(key02, bSecondKeyBinding);
        }
        ImGui::PopID();

        if (bSecondKeyBinding)
            input.KeyBind(key02, bSecondKeyBinding);

		if (key01 == key02 && key01 != NULL)
			key02 = NULL;
        else {
            g.dwAimKey0 = key01;
            g.dwAimKey1 = key02;
        }

        printf("%d, %d\n", key01, key02);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);

        Custom::EndTwoColumnPage();
    }

    // Page 2
    if (Custom::BeginTwoColumnPage(1, theme))
    {
        Custom::BeginSection("Visual", theme);
        /*--------------------------------------------------------*/
        
        Custom::Toggle("ESP", &g.VisualEnable, theme);
        //Custom::Toggle("Glow", &g.GlowEnable, theme);
        Custom::Toggle("Radar", &g.RadarEnable, theme);

        Custom::Checkbox("NPC ESP", &g.ESP_NPC, theme);
        Custom::Checkbox("Team ESP", &g.ESP_Team, theme);

        ImGui::Dummy(ImVec2(0.f, 8.f));
		Custom::SectionHeader("ESP Options", theme);

        Custom::Checkbox("Box", &g.bBox, theme);
        Custom::Checkbox("BoxFilled", &g.bFilled, theme);
        Custom::Checkbox("Line", &g.bLine, theme);
        Custom::Checkbox("Skeleton", &g.bSkeleton, theme);
        Custom::Checkbox("HealthBar", &g.bHealth, theme);
        Custom::Checkbox("Name", &g.bName, theme);
        Custom::Checkbox("Distance", &g.bDistance, theme);
        Custom::Checkbox("Weapon", &g.bWeapon, theme);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::TwoColumnSplit(theme);
        Custom::BeginSection("Visual Setting", theme);
        /*--------------------------------------------------------*/

        Custom::Combo("GlowMode", &g.GlowStyle, GlowStyleList, IM_ARRAYSIZE(GlowStyleList), theme);
        Custom::Combo("BoxMode", &g.ESP_BoxRenderMode, BoxRenderModeList, IM_ARRAYSIZE(BoxRenderModeList), theme);
        Custom::Combo("BoxType", &g.ESP_BoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList), theme);

        Custom::SliderInt("Distance", &g.ESP_MaxDistance, 10, 2000, theme);
        Custom::SliderFloat("RadarScale", &g.RadarScale, 1, 50, theme);

		Custom::SectionHeader("Colors", theme);
        ImGui::ColorEdit3("Enemy", &g.Color_ESP_Enemy.Value.x);
        ImGui::ColorEdit3("Visible", &g.Color_ESP_Visible.Value.x);
        ImGui::ColorEdit3("AimTarget", &g.Color_ESP_AimTarget.Value.x);
        ImGui::ColorEdit3("Team", &g.Color_ESP_Team.Value.x);
        ImGui::ColorEdit3("Shadow", &g.Color_ESP_Shadow.Value.x, ImGuiColorEditFlags_DisplayRGB);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::EndTwoColumnPage();
    }

    // Page 3
    if (Custom::BeginTwoColumnPage(2, theme))
    {
        Custom::BeginSection("Exploit / Misc", theme);
        /*--------------------------------------------------------*/

        //ImGui::Checkbox("BunnyHop", &g.g_bHop);
        Custom::Toggle("RCS", &g.RecoilControllSystem, theme);
        Custom::Toggle("SKinChanger", &g.bSkinChanger, theme);

        ImGui::Dummy(ImVec2(0.f, 8.f));
        Custom::SectionHeader("RCS", theme);
        Custom::Checkbox("RCS", &g.RecoilControllSystem, theme);
        Custom::SliderFloat("Scale", &g.RCS_Scale, 0.f, 1.f, theme);

        ImGui::Dummy(ImVec2(0.f, 8.f));
        Custom::SectionHeader("SkinChanger", theme);
        Custom::SliderInt("Body", &m_bodySkinId, 0, 30, theme);
        Custom::SliderInt("Weapon", &m_weaponSkinId, 0, 30, theme);

        ImGui::Dummy(ImVec2(0.f, 8.f));
        Custom::SectionHeader("AimAssist", theme);

        Custom::SliderFloat("AimAssist", &g.AimAssistMod, 0.f, 1.f, theme);

        if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            m.Write<float>(m.m_dwClientBaseAddr + offset::AimAssistVal, g.AimAssistMod);

        ImGui::EndChild();
        ImGui::BeginChild("##C022", ImVec2(ImGui::GetContentRegionAvail()), true);

        Custom::SectionHeader("SkinChanger", theme);
        Custom::Checkbox("SKinChanger", &g.bSkinChanger, theme);
        Custom::SliderInt("Body ID", &m_bodySkinId, 0, 15, theme);
        Custom::SliderInt("Weapon ID", &m_weaponSkinId, 0, 10, theme);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::TwoColumnSplit(theme);
        Custom::BeginSection("Any.", theme);
        /*--------------------------------------------------------*/



        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::EndTwoColumnPage();
    }

    // Page 4
    if (Custom::BeginTwoColumnPage(3, theme))
    {
        Custom::BeginSection("Config", theme);
        /*--------------------------------------------------------*/
        
        static int configIndex = 0;
        
        // 設定ファイルのリストを作成
        std::vector<std::string> configs = config.GetConfigList();

        if (configIndex >= static_cast<int>(configs.size()))
            configIndex = configs.empty() ? 0 : static_cast<int>(configs.size()) - 1;

        std::vector<const char*> items;
        items.reserve(configs.size());

        for (const std::string& configName : configs)
            items.push_back(configName.c_str());

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::ListBox("##List", &configIndex, items.data(), static_cast<int>(items.size()));

		if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 4.f, 20.f)) && !items.empty())
		{
			config.SaveSetting(items[configIndex]);
		}

        ImGui::SameLine();

        if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)) && !items.empty())
        {
            config.LoadSetting(items[configIndex]);
        }

        if (!g.GenerateFlag)
        {
            if (ImGui::Button("Generate ConfigFile", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            {
                g.newConfigName.clear();
                g.GenerateFlag = true;
            }
        }

        if (g.GenerateFlag)
        {
            static std::array<char, 64> configNameBuffer{};

			input.EnableKeyboardHook();

            ImGui::Text("Config name:");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            const bool submitByEnter = ImGui::InputText("##new_config_name", configNameBuffer.data(), configNameBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue);
            g.newConfigName = configNameBuffer.data();

            const float halfWidth = ImGui::GetContentRegionAvail().x / 2.f - 4.f;
            if (ImGui::Button("OK", ImVec2(halfWidth, 20.f)) || submitByEnter)
            {
                if (!g.newConfigName.empty())
                    config.CreateConfig(g.newConfigName);

                input.DisableKeyboardHook();
                configNameBuffer.fill('\0');
                g.newConfigName.clear();
                g.GenerateFlag = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
            {
				input.DisableKeyboardHook();
                configNameBuffer.fill('\0');
                g.newConfigName.clear();
                g.GenerateFlag = false;
            }

        }

        if (deleteFlag)
        {
            ImGui::Text("Delete this file?");

            if (ImGui::Button("OK", ImVec2(90.f, 20.f))) {
                if (!items.empty()) {
                    config.DeleteConfig(items[configIndex]);
                }
                deleteFlag = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(90.f, 20.f)))
                deleteFlag = false;
        }

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::TwoColumnSplit(theme);
        Custom::BeginSection("Any.", theme);
        /*--------------------------------------------------------*/



        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::EndTwoColumnPage();
    }

    // Page 4
    if (Custom::BeginTwoColumnPage(4, theme))
    {
        Custom::BeginSection("Setting", theme);
        /*--------------------------------------------------------*/

		Custom::Toggle("SpectatorList", &g.SpectatorListEnable, theme);

        Custom::Checkbox("Crosshair", &g.CrosshairEnable, theme);

		Custom::SliderInt("MaxFPS", &g.MaxFramerate, 30, 500, theme);


		Custom::SectionHeader("Crosshair", theme);

        Custom::Checkbox("Enable##Crosshair", &g.CrosshairEnable, theme);
        Custom::SliderInt("CrosshairSize", &g.CrosshairSize, 1, 10, theme);
        ImGui::ColorEdit4("Color##C", &g.Color_Crosshair.Value.x);
        Custom::Combo("Type##C", &g.CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList), theme);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::TwoColumnSplit(theme);
        Custom::BeginSection("Any.", theme);
        /*--------------------------------------------------------*/

        if (Custom::Button("Exit", theme))
            g_ApplicationActive.store(false);

        /*--------------------------------------------------------*/
        Custom::EndSection(theme);
        Custom::EndTwoColumnPage();
    }

    Custom::EndContentArea();
    Custom::PopTheme();
    ImGui::End();
}
#include "CFramework.h"
#define M_PI 3.14159265358979323846

CEntity lastTarget = CEntity();
CEntity lockedTarget = CEntity();
auto g_gui = std::make_unique<Renderer>();

bool InScreen(const BoundingBox* box)
{
    return !(box->top == 0 && box->bottom == 0 && box->left == 0 && box->right == 0);
}

void CFramework::RenderInfo()
{
    // FPS
    g_gui->String(Vector2(3.f, 3.f), TEXT_COLOR_DEFAULT, 1.f, std::to_string((int)ImGui::GetIO().Framerate).c_str());

    // FOV Circle
    if (g.AimBotEnable && g.bShowFOV)
        g_gui->Circle(Vector2((g.rcSize.right / 2.f), (g.rcSize.bottom / 2.f)), g.AimFOV, g.bRainbowFOV ? g_gui->ApplyAlpha(g_gui->GenerateRainbow(), 0.35f) : g.Color_AimFOV);

    // Crosshair
    if (g.CrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.rcSize.right / 2, g.rcSize.bottom / 2);
            ImColor crosshair_col = g_gui->ApplyAlpha(g.Color_Crosshair, g.m_flGlobalAlpha);

            g_gui->Line(Vector2(Center.x - g.CrosshairSize, Center.y), Vector2((Center.x + g.CrosshairSize) + 1, Center.y), crosshair_col, 1.f);
            g_gui->Line(Vector2(Center.x, Center.y - g.CrosshairSize), Vector2(Center.x, (Center.y + g.CrosshairSize) + 1), crosshair_col, 1.f);
        }   break;
        case 1:
            g_gui->CircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), 0.85f); // 0.85f == CrosshairAlpha
            g_gui->CircleFilled(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.CrosshairSize, g.Color_Crosshair, 0.85f);
            break;
        }
    }

    // SpectatorList
    auto spectator_list = GetSpectatorList();

    if (g.SpectatorListEnable && spectator_list.size() != 0)
    {
        g_gui->String(Vector2(g.rcSize.right / 2 - (ImGui::CalcTextSize("[ Spectator Found! ]").x / 2), g.rcSize.top), TEXT_COLOR_WARNING, 1.f, "[ Spectator Found! ]");

        ImGui::SetNextWindowBgAlpha(spectator_list.size() > 0 ? 0.9f : 0.35f);

        static bool first = true;
        if (first) {
            ImGui::SetNextWindowPos(ImVec2(12.f, 16.f));
            ImGui::SetNextWindowSize(ImVec2(250.f, 125.f));
            first = !first;
        }

        ImGui::Begin("Spectator(s)", &g.bShowMenu, ImGuiWindowFlags_NoCollapse);

        for (const auto& name : spectator_list)
            ImGui::Text(name.c_str());

        ImGui::End();
    }
}

void CFramework::RenderESP()
{
    // AimBot
    CEntity target = CEntity();
    float MinFov{ FLT_MAX };
    float MinDistance{ FLT_MAX };
    const Vector2 ScreenMiddle{ g.rcSize.right / 2.f, g.rcSize.bottom / 2.f };

    // Local
    CEntity local = GetLocal();

    if (!local.Update())
        return;

    // ViewMatrix and more
    const float baseTime = local.GetTimeBase();
    const uintptr_t ViewRenderer = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::ViewRender);
    const uintptr_t pViewMatrix = m.Read<uintptr_t>(ViewRenderer + offset::ViewMatrix);
    Matrix ViewMatrix = m.Read<Matrix>(pViewMatrix);
    const uintptr_t entitylist = m.m_dwClientBaseAddr + offset::dwEntityList;

    // 2D Radar
    const float s_radar_scale{ 12.f };
    const Vector2 s_radar_size{ 250.f, 250.f };
    const Vector2 s_radar_pos{ 25.f, g.rcSize.bottom - (s_radar_size.y + 25.f) };
    const Vector2 s_radar_center{ s_radar_pos.x + s_radar_size.x / 2.f, s_radar_pos.y + s_radar_size.y / 2.f };

    // Radar
    g_gui->Rect(Vector2(s_radar_pos), Vector2(s_radar_pos + s_radar_size), ImColor(1.f, 1.f, 1.f, 0.5f));
    g_gui->Line(Vector2(s_radar_center.x, s_radar_pos.y), Vector2(s_radar_center.x, s_radar_pos.y + s_radar_size.y), ImColor(1.f, 1.f, 1.f, 0.5f), 1.f);
    g_gui->Line(Vector2(s_radar_pos.x, s_radar_center.y), Vector2(s_radar_pos.x + s_radar_size.x, s_radar_center.y), ImColor(1.f, 1.f, 1.f, 0.5f), 1.f);
    g_gui->CircleFilled(s_radar_center, 3.f, g.Color_ESP_Team, 1.f);

    // Circle
    for (int r = 1; 3 > r; r++)
        g_gui->Circle(s_radar_center, 50.f * r, ImColor(1.f, 1.f, 1.f, 0.5f));

    for (auto& entity : this->GetEntityList())
    {
        if (!entity.Update())
            continue;

        // 距離を取得
        const float flDistance = ((local.m_vecAbsOrigin - entity.m_vecAbsOrigin).Length() * 0.01905f);

        // 各種チェック
        if (g.ESP_MaxDistance < flDistance)
            continue;

        ImColor color = g.Color_ESP_Enemy;
        BoundingBox box = entity.GetBoundingBoxData(ViewMatrix);

        if (InScreen(&box))
        {
            // サイズ算出
            const int Height = box.bottom - box.top;
            const int Width = box.right - box.left;
            const int Center = (box.right - box.left) / 2.f;
            const int bScale = (box.right - box.left) / 3.f;

            // 色を決める
            ImColor shadow_color = g_gui->ApplyAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha);
            ImColor tempColor = local.m_iTeamNum == entity.m_iTeamNum ? g.Color_ESP_Team : entity.IsVisible(baseTime) ? g.Color_ESP_Visible : g.Color_ESP_Enemy;

            if (entity.m_address == lastTarget.m_address)
                tempColor = g.Color_ESP_AimTarget;

            color = g_gui->ApplyAlpha(tempColor, g.m_flGlobalAlpha);

            // Glow
            switch (g.GlowStyle)
            {
            case 0:
                if (m.Read<int>(entity.m_address + 0x310) != 0)
                    entity.DisableGlow();
                break;
            case 1:
                entity.EnableGlow(GlowColor{ color.Value.x, color.Value.y, color.Value.z }, GlowMode{ 101, 6, 85, 96 });
                break;
            default:
                break;
            }

            // TeamCheck
            if (!g.ESP_Team && entity.m_iTeamNum == local.m_iTeamNum)
                continue;

            // Line
            if (g.bLine)
                g_gui->Line(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(box.right - (Width / 2), box.bottom), color, g.m_flGlobalAlpha);

            BoundingBox bbox{};

            // Box
            if (g.bBox)
            {
                switch (g.ESP_BoxRenderMode)
                {
                case 0: {
                    // Head bone
                    Vector2 pBase{}, pHead{};
                    const Vector3 Head = entity.GetBoneByID(8) + Vector3(0.f, 0.f, 12.f);
                    if (!WorldToScreen(ViewMatrix, g.rcSize, entity.m_vecAbsOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, g.rcSize, Head, pHead))
                        continue;

                    int height = pBase.y - pHead.y;
                    int width = height / 3;

                    bbox.left = pBase.x - width;
                    bbox.top = pHead.y;
                    bbox.bottom = pBase.y;
                    bbox.right = pBase.x + width;
                }   break;
                case 1: {
                    // BoundingBox
                    bbox = entity.GetBoundingBoxData(ViewMatrix);
                }   break;
                default:
                    break;
                }

                // BoxFilled
                if (g.bFilled)
                    g_gui->RectFilled(box.left, box.top, box.right, box.bottom, g_gui->ApplyAlpha(shadow_color, g.m_flShadowAlpha));

                switch (g.ESP_BoxType)
                {
                case 0:
                    g_gui->Rect(Vector2(box.left, box.top), Vector2(box.right, box.bottom), color);
                    break;
                case 1:
                    g_gui->CorneredBox(Vector2(box.left, box.top), Vector2(box.right, box.bottom), bScale, color);
                    break;
                }
            }

            // Skeleton - ToDo
            //if (g.bSkeleton) 
            {
                //uintptr_t pStudioHdr = m.Read<uintptr_t>(entity.m_address + 0x10E0);
                //uintptr_t StudioHdr = m.Read<uintptr_t>(pStudioHdr + 0x8);
            }

            // Health/Shieldbar
            if (g.bHealth)
            {
                g_gui->Healthbar(Vector2(box.left - 3, box.top + 1), Vector2(box.left - 2, box.bottom - 1), entity.m_iHealth , entity.m_iMaxHealth, g.Color_ESP_Shadow, g.m_flGlobalAlpha);

                if (entity.m_shieldHealth >= 0)
                    g_gui->Shieldbar(Vector2(box.left - 7, box.top + 1), Vector2(box.left - 6, box.bottom - 1), entity.m_shieldHealth, entity.m_shieldHealthMax, shadow_color, g.m_flGlobalAlpha);
            }

            // Name
            if (g.bName)
                g_gui->StringEx(Vector2(box.right - Center - (ImGui::CalcTextSize(entity.m_szName).x / 2.f), box.top - ImGui::GetFontSize()), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), entity.m_szName);

            // Distance & Weapon
            std::string outStr{};

            // Distance
            if (g.bDistance)
                outStr += "[ " + std::to_string((int)flDistance) + "m ]";

            // Rendering
            if (g.bDistance || g.bWeapon && outStr.size() > 0)
                g_gui->StringEx(Vector2(box.right - Center - (ImGui::CalcTextSize(outStr.c_str()).x / 2.f), box.bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), outStr.c_str());
        }

        // 2D Radar
        //if (g.ESP_Radar)
        {
            Vector3 delta = entity.m_vecAbsOrigin - local.m_vecAbsOrigin;
            float yaw = local.GetViewAngle().y * (M_PI / 180.f); // ToRadian
            float cosYaw = cosf(yaw);
            float sinYaw = sinf(yaw);

            Vector2 rotated{
                delta.y * cosYaw - delta.x * sinYaw,
                delta.y * sinYaw + delta.x * cosYaw
            };

            rotated /= 12.f;
            rotated *= -1.f;
            rotated += s_radar_center;
            rotated.x = std::clamp(rotated.x, s_radar_pos.x, s_radar_pos.x + s_radar_size.x);
            rotated.y = std::clamp(rotated.y, s_radar_pos.y, s_radar_pos.y + s_radar_size.y);

            g_gui->CircleFilled(rotated, 3.f, color, 1.f);
        }
        
        if (flDistance > g.AimMaxDistance)
            continue;

        // AimBot
        if (g.AimBotEnable && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode) && InScreen(&box))
        {
            if (g.bVisCheck && !entity.IsVisible(baseTime))
                continue;

            // TargetBone
            int boneId = 1;
            switch (g.AimTargetBone)
            {
            case 0: boneId = 8; break;
            case 1: boneId = 3; break;
            case 2: boneId = 2; break;
            default:
                break;
            }

            Vector2 boneCheck{};
            if (WorldToScreen(ViewMatrix, g.rcSize, entity.GetBoneByID(boneId), boneCheck))
            {
                float FOV = abs((ScreenMiddle - boneCheck).Length());

                if (FOV < g.AimFOV * 1.1f)
                {
                    switch (g.AimMode)
                    {
                    case 0: // Crosshair
                        if (MinFov > FOV) {
                            if (target.m_address == NULL || MinDistance > flDistance)
                            {
                                target = entity;
                                MinFov = FOV;
                                MinDistance = flDistance;
                            }
                        }
                        break;
                    case 1: // Game Distance
                        if (MinDistance > flDistance) {
                            target = entity;
                            MinDistance = flDistance;
                        }
                        break;
                    }

                    break;
                }
            }
        }
        else
        {
            lastTarget = CEntity();
        }
    }

    // AimBot - ToDo
    if (target.m_address != NULL && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode) && !m.Read<bool>(m.m_dwClientBaseAddr + offset::bIsMenuOpened))
    {
        if (g.hGameWindow == GetForegroundWindow())
        {
            int boneId = 1;
            switch (g.AimTargetBone)
            {
            case 0: boneId = 8; break;
            case 1: boneId = 3; break;
            case 2: boneId = 2; break;
            default:
                break;
            }

            // Simple prediction
            auto targetPos = target.GetBoneByID(boneId);
            const float distance = ((local.m_vecAbsOrigin - targetPos).Length() * 0.01905f);

            uintptr_t latestWeapon = local.GetCurrentWeapon(entitylist);
            
            float speed = m.Read<float>(latestWeapon + 0x1CB8);
            float gravity = m.Read<float>(latestWeapon + 0x1650);

            Vector3 predict{ 0, 0, 0 };

            if (speed > 1.f)
            {
                float bulletTime = distance / speed;
                predict.x = target.m_vecAbsVelocity.x * bulletTime;
                predict.y = target.m_vecAbsVelocity.y * bulletTime;
                predict.z = (700.f *  gravity * 0.5f) * (bulletTime * bulletTime);
            }

            targetPos += predict;

            Vector2 Angle = CalcAngle(local.camera_origin, targetPos);
            Vector2 ViewAngle = local.GetViewAngle();
            Vector2 Delta{};

            // NoSway
            if (g.bRemoveSway)
            {
                Vector2 Breath = local.GetSwayAngle() - ViewAngle;

                if (Breath.x != 0.f || Breath.y != 0.f)
                    Delta = (Angle - ViewAngle) - Breath;
            }
   
            NormalizeAngles(Delta);
            Vector2 SmoothedAngle = ViewAngle + (Delta / g.AimSmooth);
            NormalizeAngles(SmoothedAngle);

            if (!Vec2_Empty(SmoothedAngle))
                local.SetViewAngle(SmoothedAngle);
        }

        lastTarget = target;
    }
}
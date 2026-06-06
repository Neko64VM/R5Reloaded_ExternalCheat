#include "CFramework.h"
#include "../Framework/Memory/Memory.h"
#include <corecrt_math_defines.h>

CEntity lastTarget{ 0 };
CEntity lockedTarget{ 0 };

// 画面上中央に表示する観戦者警告のテキスト
constexpr char spectator_warning_text[32]{ "[ Spectator Found! ]" };

bool InScreen(const BoundingBox* box) noexcept
{
    return !(box->top == 0 && box->bottom == 0 && box->left == 0 && box->right == 0);
}

void CFramework::RenderInfo()
{
    // FPS
    std::string framerate = std::to_string((int)ImGui::GetIO().Framerate) + "FPS";
    m_gui->String(Vector2(3.f, 3.f), TEXT_COLOR_DEFAULT, 1.f, framerate.c_str());

    m_screenSize = g.gameSize;

    // FOV Circle
    if (g.AimBotEnable && g.bShowFOV)
        m_gui->Circle(Vector2((m_screenSize.x / 2.f), (m_screenSize.y / 2.f)), g.AimFOV, g.bRainbowFOV ? m_gui->ApplyAlpha(m_gui->GenerateRainbow(), 0.35f) : g.Color_AimFOV);

    // Crosshair
    if (g.CrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 screenCenter = ImVec2(m_screenSize.x / 2, m_screenSize.y / 2);
            ImColor ch_color = m_gui->ApplyAlpha(g.Color_Crosshair, g.m_flGlobalAlpha);

            m_gui->Line(Vector2(screenCenter.x - g.CrosshairSize, screenCenter.y), Vector2((screenCenter.x + g.CrosshairSize) + 1, screenCenter.y), ch_color, 1.f);
            m_gui->Line(Vector2(screenCenter.x, screenCenter.y - g.CrosshairSize), Vector2(screenCenter.x, (screenCenter.y + g.CrosshairSize) + 1), ch_color, 1.f);
        }   break;
        case 1:
            m_gui->CircleFilled(Vector2(m_screenSize.y / 2.f, m_screenSize.y / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), 0.85f); // 0.85f == CrosshairAlpha
            m_gui->CircleFilled(Vector2(m_screenSize.x / 2.f, m_screenSize.y / 2.f), g.CrosshairSize, g.Color_Crosshair, 0.85f);
            break;
        }
    }

    if (g.SpectatorListEnable && m_GameDataSnapshot.m_spectatorlist.size() != 0)
    {
        m_gui->String(Vector2(m_screenSize.x / 2 - (ImGui::CalcTextSize(spectator_warning_text).x / 2), 2.f), TEXT_COLOR_WARNING, 1.f, spectator_warning_text);

        // 初回でウィンドウ位置を決める
        static bool first = true;
        if (first) {
            ImGui::SetNextWindowPos(ImVec2(12.f, 16.f));
            ImGui::SetNextWindowSize(ImVec2(250.f, 125.f));
            first = false;
        }

        ImGui::SetNextWindowBgAlpha(m_GameDataSnapshot.m_spectatorlist.size() > 0 ? 0.9f : 0.35f);
        ImGui::Begin("Spectator(s)", &g.bShowMenu, ImGuiWindowFlags_NoCollapse);

        for (const auto& name : m_GameDataSnapshot.m_spectatorlist) {
            ImGui::Text(name.c_str());
        }

        ImGui::End();
    }
}

void CFramework::RenderESP()
{
    // UpdateLocal
    if (!m_GameDataSnapshot.m_localplayer.Update())
        return;
    
    // 各リソースの準備
    CEntity target = CEntity(0);
    float minFov{ FLT_MAX };
    float minDistance{ FLT_MAX };
    const Vector2 ScreenCenter{ m_screenSize.x / 2.f, m_screenSize.y / 2.f };

	// ViewMatrixの取得
    uintptr_t ViewRenderer = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::ViewRender); // ViewMatrix
    uintptr_t pViewMatrix = m.Read<uintptr_t>(ViewRenderer + offset::ViewMatrix);
    Matrix ViewMatrix = m.Read<Matrix>(pViewMatrix);

    const uintptr_t entitylist = m.m_dwClientBaseAddr + offset::dwEntityList;
    const float baseTime = m_GameDataSnapshot.m_localplayer.GetTimeBase(); // ClientState辺りのものに置き換えられない？

    // 2D Radar
    const float s_radar_scale{ 12.f };
    const Vector2 s_radar_size{ 250.f, 250.f };
    const Vector2 s_radar_pos{ 25.f,m_screenSize.y - (s_radar_size.y + 25.f) };
    const Vector2 s_radar_center{ s_radar_pos.x + s_radar_size.x / 2.f, s_radar_pos.y + s_radar_size.y / 2.f };

    // Radar frame
    if (g.RadarEnable)
    {
        //m_gui->Rect(Vector2(s_radar_pos), Vector2(s_radar_pos + s_radar_size), COLOR_RADAR_FRAME);
        m_gui->Line(Vector2(s_radar_center.x, s_radar_pos.y), Vector2(s_radar_center.x, s_radar_pos.y + s_radar_size.y), COLOR_RADAR_FRAME, 1.f);
        m_gui->Line(Vector2(s_radar_pos.x, s_radar_center.y), Vector2(s_radar_pos.x + s_radar_size.x, s_radar_center.y), COLOR_RADAR_FRAME, 1.f);
        m_gui->CircleFilled(s_radar_center, 3.f, g.Color_ESP_Team, 1.f);

        for (int r = 1; 3 > r; r++) {
            m_gui->Circle(s_radar_center, 50.f * r, COLOR_RADAR_FRAME);
        }
    }

    for (auto& entity : m_GameDataSnapshot.m_entitylist)
    {
        if (!entity.Update())
            continue;

        // 距離を取得
        const float flDistance = (m_GameDataSnapshot.m_localplayer.m_vecAbsOrigin - entity.m_vecAbsOrigin).Length() * 0.01905f;

        // 各種チェック
        if (g.ESP_MaxDistance < flDistance)
            continue;
        
        ImColor color = g.Color_ESP_Enemy;
        BoundingBox box = entity.GetBoundingBoxData(ViewMatrix, m_screenSize);

        // 正面180°以内に敵がいたらレンダリング
        if (InScreen(&box))
        {
            // サイズ算出
            const int Height = box.bottom - box.top;
            const int Width = box.right - box.left;
            const int Center = (box.right - box.left) / 2.f;
            const int bScale = (box.right - box.left) / 3.f;

            // 色を決める
            ImColor shadow_color = m_gui->ApplyAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha);
            ImColor tempColor = m_GameDataSnapshot.m_localplayer.m_iTeamNum == entity.m_iTeamNum ? g.Color_ESP_Team : entity.IsVisible(baseTime) ? g.Color_ESP_Visible : g.Color_ESP_Enemy;

            if (entity.m_address == lastTarget.m_address)
                tempColor = g.Color_ESP_AimTarget;

            color = m_gui->ApplyAlpha(tempColor, g.m_flGlobalAlpha);

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
            if (!g.ESP_Team && entity.m_iTeamNum == m_GameDataSnapshot.m_localplayer.m_iTeamNum)
                continue;

            // Line
            if (g.bLine)
                m_gui->Line(Vector2(m_screenSize.x / 2.f, m_screenSize.y), Vector2(box.right - (Width / 2), box.bottom), color, g.m_flGlobalAlpha);

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
                    if (!WorldToScreen(ViewMatrix, m_screenSize, entity.m_vecAbsOrigin + Vector3(0.f, 0.f, -6.f), pBase) || !WorldToScreen(ViewMatrix, m_screenSize, Head, pHead))
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
                    bbox = entity.GetBoundingBoxData(ViewMatrix, m_screenSize);
                }   break;
                default:
                    break;
                }

                // BoxFilled
                if (g.bFilled)
                    m_gui->RectFilled(bbox.left, bbox.top, bbox.right, bbox.bottom, m_gui->ApplyAlpha(shadow_color, g.m_flShadowAlpha));

                switch (g.ESP_BoxType)
                {
                case 0:
                    m_gui->Rect(Vector2(bbox.left, bbox.top), Vector2(bbox.right, bbox.bottom), color);
                    break;
                case 1:
                    m_gui->CorneredBox(Vector2(bbox.left, bbox.top), Vector2(bbox.right, bbox.bottom), bScale, color);
                    break;
                }
            }

            // Skeleton - ToDo
            //if (g.bSkeleton) 
            {
                //uintptr_t pStudioHdr = m.Read<uintptr_t>(entity.m_address + 0x...);
                //uintptr_t StudioHdr = m.Read<uintptr_t>(pStudioHdr + 0x8);
            }

            // Health/Shieldbar
            if (g.bHealth)
            {
                m_gui->Healthbar(Vector2(bbox.left - 3, bbox.top + 1), Vector2(bbox.left - 2, bbox.bottom - 1), entity.m_iHealth , entity.m_iMaxHealth, g.Color_ESP_Shadow, g.m_flGlobalAlpha);

                if (entity.m_shieldHealth >= 0)
                    m_gui->Shieldbar(Vector2(bbox.left - 7, bbox.top + 1), Vector2(bbox.left - 6, bbox.bottom - 1), entity.m_shieldHealth, entity.m_shieldHealthMax, shadow_color, g.m_flGlobalAlpha);
            }

            // Name
            if (g.bName)
                m_gui->StringEx(Vector2(bbox.right - Center - (ImGui::CalcTextSize(entity.m_szName).x / 2.f), bbox.top - ImGui::GetFontSize()), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), entity.m_szName);

            // Distance & Weapon
            std::string szResult{};

            // Weapon
            if (g.bWeapon) {
                szResult += entity.GetWeaponName(entitylist);
            }
            
            // Distance
            if (g.bDistance)
                szResult += " [ " + std::to_string((int)flDistance) + "m ]";

            // Rendering
            if (g.bDistance || g.bWeapon && szResult.size() > 0)
                m_gui->StringEx(Vector2(box.right - Center - (ImGui::CalcTextSize(szResult.c_str()).x / 2.f), box.bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), szResult.c_str());
        }

        // 2D Radar
        if (g.RadarEnable)
        {
            Vector3 delta = entity.m_vecAbsOrigin - m_GameDataSnapshot.m_localplayer.m_vecAbsOrigin;
            float yaw = m_GameDataSnapshot.m_localplayer.GetViewAngle().y * (M_PI / 180.f); // ラジアンに変換
            float cosYaw = cosf(yaw);
            float sinYaw = sinf(yaw);

            Vector2 rotated {
                delta.y * cosYaw - delta.x * sinYaw,
                delta.y * sinYaw + delta.x * cosYaw };

            rotated /= g.RadarScale;
            rotated *= -1.f; // XとY軸を逆転させる
            rotated += s_radar_center;
            rotated.x = std::clamp(rotated.x, s_radar_pos.x, s_radar_pos.x + s_radar_size.x); // 四角形の外にレンダリングされないようにする
            rotated.y = std::clamp(rotated.y, s_radar_pos.y, s_radar_pos.y + s_radar_size.y);

            m_gui->CircleFilled(rotated, 3.f, color, 1.f);
        }
        
        if (flDistance > g.AimMaxDistance)
            continue;
        
        if (!g.bAimAtTeam && entity.m_iTeamNum == m_GameDataSnapshot.m_localplayer.m_iTeamNum)
            continue;

        // AimBot - Something's wrong
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
            if (WorldToScreen(ViewMatrix, m_screenSize, entity.GetBoneByID(boneId), boneCheck))
            {
                float FOV = abs((ScreenCenter - boneCheck).Length());

                if (FOV < g.AimFOV)
                {
                    switch (g.AimMode)
                    {
                    case 0: // Crosshair
                        if (minFov > FOV) {
                            target = entity;
                            minFov = FOV;
                        }
                        break;
                    case 1: // Game Distance
                        if (minDistance > flDistance) {
                            target = entity;
                            minDistance = flDistance;
                        }
                        break;
                    }

                    continue;
                }
            }
        }
        else
        {
            lastTarget = CEntity(0);
        }
    }

    // AimBot - ToDo
    if (target.m_address != NULL && AimBotKeyCheck(g.dwAimKey0, g.dwAimKey1, g.AimKeyMode) && !m.Read<bool>(m.m_dwClientBaseAddr + offset::bIsMenuOpened))
    {
        int boneId = 1;

        switch (g.AimTargetBone)
        {
        case 0: boneId = 8; break;
        case 1: boneId = 3; break;
        default:
            break;
        }

        // Simple prediction
        Vector3 targetPos = target.GetBoneByID(boneId);
        const float distance = ((m_GameDataSnapshot.m_localplayer.m_vecAbsOrigin - targetPos).Length() * 0.01905f);

        uintptr_t latestWeapon = m_GameDataSnapshot.m_localplayer.GetCurrentWeapon(entitylist);

        float speed = m.Read<float>(latestWeapon + 0x1CB8);
        float gravity = m.Read<float>(latestWeapon + 0x1C44) * 1000; 

        Vector3 predict{ 0, 0, 0 };

        if (speed > 1.f)
        {
            float bulletTime = distance / speed;
            predict.x = target.m_vecAbsVelocity.x * bulletTime;
            predict.y = target.m_vecAbsVelocity.y * bulletTime;
            predict.z = ((gravity * 750.f) * 0.5f) * (bulletTime * bulletTime);
        }

        targetPos += predict;

        Vector2 Angle = CalcAngle(m_GameDataSnapshot.m_localplayer.camera_origin, targetPos);
        Vector2 ViewAngle = m_GameDataSnapshot.m_localplayer.GetViewAngle();
        Vector2 Delta{};

        // NoSway
        if (g.bRemoveSway)
        {
            Vector2 AimSway = m_GameDataSnapshot.m_localplayer.GetSwayAngle() - ViewAngle;

            if (AimSway.x != 0.f || AimSway.y != 0.f)
                Delta = (Angle - ViewAngle) - AimSway;
        }

        NormalizeAngles(Delta);
        Vector2 SmoothedAngle = ViewAngle + (Delta / g.AimSmooth);
        NormalizeAngles(SmoothedAngle);

        if (!Vec2_Empty(SmoothedAngle))
            m_GameDataSnapshot.m_localplayer.SetViewAngle(SmoothedAngle);

        lastTarget = target;
    }
}
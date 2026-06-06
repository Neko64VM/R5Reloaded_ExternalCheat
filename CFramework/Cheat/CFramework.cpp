#include "CFramework.h"
#include "../Framework/Utils.hpp"

auto gui = std::make_unique<CRenderer>();

bool CFramework::Init()
{
    m_gui->ApplyStyle();

    return true;
}

void CFramework::UpdateSnapshot()
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_GameDataSnapshot = m_NextGameDataSnapshot;
}

void CFramework::MultiFeatures()
{
    const uintptr_t entitylist = m.m_dwClientBaseAddr + offset::dwEntityList;

    // RCS
    if (g.RecoilControllSystem)
    {
        static Vector2 OldPunch{};
        Vector2 PunchAngle = m_GameDataSnapshot.m_localplayer.GetWeaponPunchAngle();

        if (!Vec2_Empty(PunchAngle)) {
            Vector2 Delta = m_GameDataSnapshot.m_localplayer.GetViewAngle() + ((OldPunch - PunchAngle) * g.RCS_Scale);
            NormalizeAngles(Delta);

            if (!Vec2_Empty(Delta))
                m_GameDataSnapshot.m_localplayer.SetViewAngle(Delta);

            OldPunch = PunchAngle;
        }
    }

    // SkinChanger
    if (g.bSkinChanger)
    {
        m.Write<int>(m_GameDataSnapshot.m_localplayer.m_address + offset::m_nSkin, m_bodySkinId);
        m.Write<int>(m_GameDataSnapshot.m_localplayer.GetCurrentWeapon(entitylist) + offset::m_nSkin, m_weaponSkinId);
    }

    GlowMode GMode{ 0, 0, 0, 0 };
    ImColor ColRainbow = gui->GenerateRainbow(g.VMG_Rate);

    switch (g.VMG_Type)
    {
    case 0:
        GMode = GlowMode(0, 0, 60, 90);
        break;
    case 1:
        GMode = GlowMode(0, 6, 90, 0);
        break;
    case 2:
        GMode = GlowMode(12, 6, 90, 0);
        break;
    default:
        break;
    }

    // ViewModel - Weapon
    uintptr_t pWeaponModel = m_GameDataSnapshot.m_localplayer.GetWeaponViewModel(entitylist);

    m.Write<int>(pWeaponModel + 0x310, 1);
    m.Write<int>(pWeaponModel + 0x320, 2);
    m.Write<GlowMode>(pWeaponModel + 0x27C, GMode);
    m.Write<GlowColor>(pWeaponModel + 0x1D0, GlowColor(ColRainbow.Value.x, ColRainbow.Value.y, ColRainbow.Value.z));

    // ViewModel - Hand
    uintptr_t pHandModel = m_GameDataSnapshot.m_localplayer.GetHandViewModel(entitylist);

    m.Write<int>(pHandModel + 0x310, 1);
    m.Write<int>(pHandModel + 0x320, 2);
    m.Write<GlowMode>(pHandModel + 0x27C, GMode);
    m.Write<GlowColor>(pHandModel + 0x1D0, GlowColor(1.f, 1.f, 1.f));
}

bool CFramework::AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode)
{
    // 無条件でAimBotを有効にする
    if (AimKeyMode == 0)
        return true;

    // 前提チェック(共通)
    if (AimKey0 == NULL || !utils::IsKeyDown(AimKey0) && !utils::IsKeyDown(AimKey1) || g.bShowMenu) {
        return false;
    }

    switch (AimKeyMode)
    {
    case 1: // and
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (!utils::IsKeyDown(AimKey0) || !utils::IsKeyDown(AimKey1))
            return false;
        else if (!utils::IsKeyDown(AimKey0))
            return false;
        break;
    case 2: // or
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (utils::IsKeyDown(AimKey0) || utils::IsKeyDown(AimKey1))
            break;

        break;
    }

    return true;
}
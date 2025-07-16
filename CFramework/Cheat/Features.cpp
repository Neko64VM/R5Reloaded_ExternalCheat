#include "CFramework.h"

auto gui = std::make_unique<Renderer>();

void CFramework::MiscAll()
{
    // Local
    CEntity local = GetLocal();
    uintptr_t EntityList = m.m_dwClientBaseAddr + offset::dwEntityList;

    if (g.RecoilControllSystem)
    {
        static Vector2 OldPunch{};

        Vector2 PunchAngle = local.GetWeaponPunchAngle();

        if (!Vec2_Empty(PunchAngle)) {
            Vector2 Delta = local.GetViewAngle() + ((OldPunch - PunchAngle) * g.RCS_Scale);
            NormalizeAngles(Delta);

            if (!Vec2_Empty(Delta))
                local.SetViewAngle(Delta);

            OldPunch = PunchAngle;
        }
    }

    // ViewModel Glow
    GlowMode mode{ 0, 0, 0, 0 };
    ImColor rainbow = gui->GenerateRainbow(g.VMG_Rate);

    switch (g.VMG_Type)
    {
    case 0:
        mode = GlowMode(0, 0, 60, 90);
        break;
    case 1:
        mode = GlowMode(0, 6, 90, 0);
        break;
    case 2:
        mode = GlowMode(12, 6, 90, 0);
        break;
    default:
        break;
    }

    // ViewModel - Weapon
    uintptr_t weaponModel = local.GetWeaponViewModel(EntityList);

    m.Write<int>(weaponModel + 0x310, 1);
    m.Write<int>(weaponModel + 0x320, 2);
    m.Write<GlowMode>(weaponModel + 0x27C, mode);
    m.Write<GlowColor>(weaponModel + 0x1D0, GlowColor(rainbow.Value.x, rainbow.Value.y, rainbow.Value.z));

    // ViewModel - Hand
    uintptr_t handModel = local.GetHandViewModel(EntityList);

    m.Write<int>(handModel + 0x310, 1);
    m.Write<int>(handModel + 0x320, 2);
    m.Write<GlowMode>(handModel + 0x27C, mode);
    m.Write<GlowColor>(handModel + 0x1D0, GlowColor(1.f, 1.f, 1.f));
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
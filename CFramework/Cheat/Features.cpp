#include "CFramework.h"

void CFramework::MiscAll()
{
    CEntity local = CEntity();
    local.m_address = m.Read<uintptr_t>(m.m_dwClientBaseAddr + offset::dwLocalPlayer);

    if (!local.Update())
        return;

    local.UpdateStatic();

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

    if (g.ViewModelGlow)
    {
        auto viewmodel_list = GetViewModelList();

        GlowMode mode{ 0, 0, 0, 0 };
        ImColor rainbow = GenerateRainbow(g.VMG_Rate);

        switch (g.VMG_Type)
        {
        case 0:
            mode = GlowMode(0, 0, 60, 90);
            break;
        case 1:
            mode = GlowMode(0, 6, 127, 0);
            break;
        case 2:
            mode = GlowMode(12, 6, 127, 0);
            break;
        default:
            break;
        }

        for (auto& vmodel : viewmodel_list)
        {
            m.Write<int>(vmodel + 0x310, 1);
            m.Write<int>(vmodel + 0x320, 2);
            m.Write<GlowMode>(vmodel + 0x27C, mode);
            m.Write<GlowColor>(vmodel + 0x1D0, GlowColor(rainbow.Value.x, rainbow.Value.y, rainbow.Value.z));
        }
    }
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
#include "Overlay.h"

bool Overlay::InitOverlay(const char* targetName, InitializeMode InitMode)
{
    if (InitMode == WINDOW_TITLE || InitMode == WINDOW_CLASS) {
        g.hGameWindow = InitMode == WINDOW_TITLE ? FindWindowA(nullptr, targetName) : FindWindowA(targetName, nullptr);
    }
    else if (InitMode == PROCESS) {
        g.hGameWindow = GetTargetWindow(targetName);
    }
    else {
        MessageBoxA(nullptr, "Invalid overlay init option", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    if (!g.hGameWindow) {
        MessageBoxA(nullptr, "target process not found", "Initialize Failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
        return false;
    }

    // Get ClassName
    GetWindowTextA(g.hGameWindow, m_TargetTitle, sizeof(m_TargetTitle));

    return CreateOverlay();
}

void Overlay::OverlayManager()
{
    // Window Check
    HWND hWnd = FindWindowA(nullptr, m_TargetTitle);
    if (!hWnd) {
        g_ApplicationActive = false;
        return;
    }

    // ShowMenu Toggle
    static bool menu = false;

    if (utils::IsKeyDown(g.dwMenuKey) && !menu) {
        g.bShowMenu = !g.bShowMenu;
        menu = true;
    }
    else if (!utils::IsKeyDown(g.dwMenuKey) && menu) {
        menu = false;
    }

    // オーバーレイをゲームの上に配置
    HWND processHwnd = GetWindow(hWnd, GW_HWNDPREV);
    SetWindowPos(m_hWnd, processHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Window Style Changer
    HWND ForegroundWindow = GetForegroundWindow();
    LONG TmpLong = GetWindowLong(m_hWnd, GWL_EXSTYLE);

    if (ForegroundWindow == m_hWnd && !g.bShowMenu && DefaultStyle != TmpLong) {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, DefaultStyle);
    }
    else if (ForegroundWindow != hWnd && g.bShowMenu && MenuStyle != TmpLong) {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, MenuStyle);
    }

    // サイズを取得
    static Vector2 oldSize{};
    static POINT oldPoint{};
    RECT currentRect{};
    POINT currentPoint{};
    GetClientRect(hWnd, &currentRect);
    ClientToScreen(hWnd, &currentPoint);

    // ImGuiにマウス入力を渡す
    POINT MousePos{};
    GetCursorPos(&MousePos);
    ImGui::GetIO().MousePos = ImVec2(MousePos.x - currentPoint.x, MousePos.y - currentPoint.y);
    ImGui::GetIO().MouseDown[0] = utils::IsKeyDown(VK_LBUTTON);

    // Window Resizer
    if (currentRect.right != g.gameSize.x || currentRect.bottom != g.gameSize.y || currentPoint.x != oldPoint.x || currentPoint.y != oldPoint.y)
    {
        g.gameSize.x = currentRect.right;
        g.gameSize.y = currentRect.bottom;
        oldPoint = currentPoint;

        SetWindowPos(m_hWnd, nullptr, currentPoint.x, currentPoint.y, currentRect.right, currentRect.bottom, SWP_NOREDRAW);
    }
}

HWND Overlay::GetTargetWindow(const std::string processName)
{
    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(PROCESSENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
            break;
    }

    CloseHandle(snapShot);

    HWND hwnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hwnd, 8) != 0 || !IsWindowVisible(hwnd))
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hwnd, &ProcessID);
        if (entry.th32ProcessID == ProcessID)
            return hwnd;
    } while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}

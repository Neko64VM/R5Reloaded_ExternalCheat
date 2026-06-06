#include "Cheat/CFramework.h"
#include "Framework/Memory/Memory.h"
#include "Framework/Overlay/Overlay.h"
#include "Framework/Config/ConfigManager.h"

auto cheat = std::make_unique<CFramework>();

void Overlay::OverlayUserFunction()
{
	cheat->UpdateSnapshot();
	cheat->MultiFeatures();
	cheat->RenderInfo();

	if (g.VisualEnable)
		cheat->RenderESP();

	if (g.bShowMenu)
		cheat->RenderMenu();
}

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	auto overlay = std::make_unique<Overlay>();

	// プロセスに接続する
	if (!m.AttachProcess("r5apex.exe", InitializeMode::PROCESS)) // 詳細は Framework/Memory/Memory.h を参照
		return 1;

	// ConfigSystem
	if (!config.InitConfigSystem("NekoHack", "R5Reloaded")) // BaseDir/MainDir
		return 2;

	// Overlay
	if (!overlay->InitOverlay("r5apex.exe", InitializeMode::PROCESS))
		return 3;

	cheat->Init();

	// スレッドを作成 - ESP/AIM用にプレイヤーのデータをキャッシュ用
	std::thread([&]() { cheat->UpdateList(); }).detach();

	// Sleep()の精度を向上させるが、システム全体に影響するので注意
	timeBeginPeriod(1);

	// MainThread
	overlay->OverlayLoop();

	// 終了する
	g_ApplicationActive.store(false);
	overlay->DestroyOverlay();
	m.DetachProcess();

	// Sleep()精度向上の解除
	timeEndPeriod(1);

	return 0;
}
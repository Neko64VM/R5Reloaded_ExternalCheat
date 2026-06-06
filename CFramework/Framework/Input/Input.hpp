#pragma once

#include <Windows.h>

#include <functional>

/*
imgui.cpp: 2489行目

例外がスローされました:読み取りアクセス違反。**data** が
LCTRL: 0x3D6
LSHIFT: 0x24D00000224
でした。
*/

class OverlayKeyboardInput
{
public:
	using KeyBindCallback = std::function<void(DWORD)>;

	OverlayKeyboardInput() = default;
	~OverlayKeyboardInput();

	OverlayKeyboardInput(const OverlayKeyboardInput&) = delete;
	OverlayKeyboardInput& operator=(const OverlayKeyboardInput&) = delete;

	bool Start();
	void Stop();
	bool IsStarted() const;

	void EnableKeyboardHook() { Start(); }
	void DisableKeyboardHook() { Stop(); }

	void BeginKeyBind(KeyBindCallback callback);
	void CancelKeyBind();
	bool IsBindingKey() const;

	void KeyBind(DWORD& targetKey, bool& isWaiting);
	static const char* GetKeyName(DWORD vkCode);

private:
	LRESULT HandleKeyboard(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMouse(int nCode, WPARAM wParam, LPARAM lParam);
	bool HandleKeyBind(DWORD vkCode);
	void ForwardToImGui(const KBDLLHOOKSTRUCT& key, WPARAM message);
	void ForwardModifierToImGui(DWORD vkCode, bool isKeyDown);

	HHOOK keyboardHook_ = nullptr;
	HHOOK mouseHook_ = nullptr;
	DWORD capturedBindKey_ = 0;
	KeyBindCallback keyBindCallback_;
	bool leftShiftDown_ = false;
	bool rightShiftDown_ = false;
	bool leftCtrlDown_ = false;
	bool rightCtrlDown_ = false;
	bool leftAltDown_ = false;
	bool rightAltDown_ = false;
	bool leftSuperDown_ = false;
	bool rightSuperDown_ = false;

	static OverlayKeyboardInput* instance_;
	static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
};

using OverlayInput = OverlayKeyboardInput;

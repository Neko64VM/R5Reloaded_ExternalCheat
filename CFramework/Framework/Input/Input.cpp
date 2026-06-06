#include "Input.hpp"

#include "../ImGui/imgui.h"

#include <utility>

namespace
{
	bool IsKeyDown(int vk)
	{
		return (GetKeyState(vk) & 0x8000) != 0;
	}

	bool IsAsciiAlphaNumeric(WCHAR ch)
	{
		return (ch >= L'0' && ch <= L'9')
			|| (ch >= L'A' && ch <= L'Z')
			|| (ch >= L'a' && ch <= L'z');
	}

	bool IsAllowedTextCharacter(WCHAR ch)
	{
		return IsAsciiAlphaNumeric(ch)
			|| ch == L'-'
			|| ch == L'_'
			|| ch == L' ';
	}

	bool IsAllowedKeyBindVirtualKey(DWORD vkCode)
	{
		if ((vkCode >= '0' && vkCode <= '9') || (vkCode >= 'A' && vkCode <= 'Z'))
			return true;
		if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
			return true;

		switch (vkCode)
		{
		case VK_LBUTTON:
		case VK_RBUTTON:
		case VK_MBUTTON:
		case VK_XBUTTON1:
		case VK_XBUTTON2:
		case VK_LSHIFT:
		case VK_LCONTROL:
		case VK_OEM_MINUS:
		case VK_SUBTRACT:
		case VK_SPACE:
		case VK_RETURN:
		case VK_ESCAPE:
			return true;
		default:
			return false;
		}
	}

	bool IsShortcutModifierDown()
	{
		return IsKeyDown(VK_CONTROL)
			|| IsKeyDown(VK_MENU)
			|| IsKeyDown(VK_LWIN)
			|| IsKeyDown(VK_RWIN);
	}

	bool IsModifierVirtualKey(DWORD vkCode)
	{
		return vkCode == VK_SHIFT
			|| vkCode == VK_LSHIFT
			|| vkCode == VK_RSHIFT
			|| vkCode == VK_CONTROL
			|| vkCode == VK_LCONTROL
			|| vkCode == VK_RCONTROL
			|| vkCode == VK_MENU
			|| vkCode == VK_LMENU
			|| vkCode == VK_RMENU
			|| vkCode == VK_LWIN
			|| vkCode == VK_RWIN;
	}

	bool IsCharacterVirtualKey(DWORD vkCode)
	{
		if ((vkCode >= '0' && vkCode <= '9') || (vkCode >= 'A' && vkCode <= 'Z'))
			return true;
		if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
			return true;

		switch (vkCode)
		{
		case VK_SPACE:
		case VK_OEM_7:
		case VK_OEM_COMMA:
		case VK_OEM_MINUS:
		case VK_OEM_PERIOD:
		case VK_OEM_2:
		case VK_OEM_1:
		case VK_OEM_PLUS:
		case VK_OEM_4:
		case VK_OEM_5:
		case VK_OEM_6:
		case VK_OEM_3:
		case VK_DECIMAL:
		case VK_DIVIDE:
		case VK_MULTIPLY:
		case VK_SUBTRACT:
		case VK_ADD:
			return true;
		default:
			return false;
		}
	}

	ImGuiKey ToImGuiKey(DWORD vk)
	{
		if (vk >= '0' && vk <= '9')
			return static_cast<ImGuiKey>(ImGuiKey_0 + (vk - '0'));
		if (vk >= 'A' && vk <= 'Z')
			return static_cast<ImGuiKey>(ImGuiKey_A + (vk - 'A'));
		if (vk >= VK_F1 && vk <= VK_F24)
			return static_cast<ImGuiKey>(ImGuiKey_F1 + (vk - VK_F1));

		switch (vk)
		{
		case VK_TAB: return ImGuiKey_Tab;
		case VK_LEFT: return ImGuiKey_LeftArrow;
		case VK_RIGHT: return ImGuiKey_RightArrow;
		case VK_UP: return ImGuiKey_UpArrow;
		case VK_DOWN: return ImGuiKey_DownArrow;
		case VK_PRIOR: return ImGuiKey_PageUp;
		case VK_NEXT: return ImGuiKey_PageDown;
		case VK_HOME: return ImGuiKey_Home;
		case VK_END: return ImGuiKey_End;
		case VK_INSERT: return ImGuiKey_Insert;
		case VK_DELETE: return ImGuiKey_Delete;
		case VK_BACK: return ImGuiKey_Backspace;
		case VK_SPACE: return ImGuiKey_Space;
		case VK_RETURN: return ImGuiKey_Enter;
		case VK_ESCAPE: return ImGuiKey_Escape;
		case VK_OEM_7: return ImGuiKey_Apostrophe;
		case VK_OEM_COMMA: return ImGuiKey_Comma;
		case VK_OEM_MINUS: return ImGuiKey_Minus;
		case VK_OEM_PERIOD: return ImGuiKey_Period;
		case VK_OEM_2: return ImGuiKey_Slash;
		case VK_OEM_1: return ImGuiKey_Semicolon;
		case VK_OEM_PLUS: return ImGuiKey_Equal;
		case VK_OEM_4: return ImGuiKey_LeftBracket;
		case VK_OEM_5: return ImGuiKey_Backslash;
		case VK_OEM_6: return ImGuiKey_RightBracket;
		case VK_OEM_3: return ImGuiKey_GraveAccent;
		case VK_CAPITAL: return ImGuiKey_CapsLock;
		case VK_SCROLL: return ImGuiKey_ScrollLock;
		case VK_NUMLOCK: return ImGuiKey_NumLock;
		case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
		case VK_PAUSE: return ImGuiKey_Pause;
		case VK_NUMPAD0: return ImGuiKey_Keypad0;
		case VK_NUMPAD1: return ImGuiKey_Keypad1;
		case VK_NUMPAD2: return ImGuiKey_Keypad2;
		case VK_NUMPAD3: return ImGuiKey_Keypad3;
		case VK_NUMPAD4: return ImGuiKey_Keypad4;
		case VK_NUMPAD5: return ImGuiKey_Keypad5;
		case VK_NUMPAD6: return ImGuiKey_Keypad6;
		case VK_NUMPAD7: return ImGuiKey_Keypad7;
		case VK_NUMPAD8: return ImGuiKey_Keypad8;
		case VK_NUMPAD9: return ImGuiKey_Keypad9;
		case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
		case VK_DIVIDE: return ImGuiKey_KeypadDivide;
		case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
		case VK_ADD: return ImGuiKey_KeypadAdd;
		case VK_APPS: return ImGuiKey_Menu;
		default: return ImGuiKey_None;
		}
	}

	void AddInputCharacter(const KBDLLHOOKSTRUCT& key)
	{
		BYTE keyboardState[256] = {};
		if (!GetKeyboardState(keyboardState))
			return;

		WCHAR chars[8] = {};
		const UINT scanCode = MapVirtualKeyW(key.vkCode, MAPVK_VK_TO_VSC);
		const int charCount = ToUnicode(key.vkCode, scanCode, keyboardState, chars, 8, 0);
		if (charCount <= 0)
			return;

		ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < charCount; ++i)
		{
			if (IsAllowedTextCharacter(chars[i]))
				io.AddInputCharacterUTF16(chars[i]);
		}
	}

	DWORD MouseMessageToVirtualKey(WPARAM message, const MSLLHOOKSTRUCT& mouse)
	{
		switch (message)
		{
		case WM_LBUTTONDOWN: return VK_LBUTTON;
		case WM_RBUTTONDOWN: return VK_RBUTTON;
		case WM_MBUTTONDOWN: return VK_MBUTTON;
		case WM_XBUTTONDOWN:
			return HIWORD(mouse.mouseData) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2;
		default:
			return 0;
		}
	}
}

OverlayKeyboardInput* OverlayKeyboardInput::instance_ = nullptr;

OverlayKeyboardInput::~OverlayKeyboardInput()
{
	Stop();
}

bool OverlayKeyboardInput::Start()
{
	if (keyboardHook_ && mouseHook_)
		return true;

	if (instance_ && instance_ != this)
		return false;

	instance_ = this;
	if (!keyboardHook_)
		keyboardHook_ = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandleW(nullptr), 0);
	if (!mouseHook_)
		mouseHook_ = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, GetModuleHandleW(nullptr), 0);

	if (!keyboardHook_ || !mouseHook_)
	{
		Stop();
		return false;
	}

	return true;
}

void OverlayKeyboardInput::Stop()
{
	if (keyboardHook_)
	{
		UnhookWindowsHookEx(keyboardHook_);
		keyboardHook_ = nullptr;
	}

	if (mouseHook_)
	{
		UnhookWindowsHookEx(mouseHook_);
		mouseHook_ = nullptr;
	}

	if (instance_ == this)
		instance_ = nullptr;

	keyBindCallback_ = nullptr;
	leftShiftDown_ = false;
	rightShiftDown_ = false;
	leftCtrlDown_ = false;
	rightCtrlDown_ = false;
	leftAltDown_ = false;
	rightAltDown_ = false;
	leftSuperDown_ = false;
	rightSuperDown_ = false;
}

bool OverlayKeyboardInput::IsStarted() const
{
	return keyboardHook_ != nullptr && mouseHook_ != nullptr;
}

void OverlayKeyboardInput::BeginKeyBind(KeyBindCallback callback)
{
	if (!Start())
		return;

	keyBindCallback_ = std::move(callback);
}

void OverlayKeyboardInput::CancelKeyBind()
{
	keyBindCallback_ = nullptr;
	capturedBindKey_ = 0;
}

bool OverlayKeyboardInput::IsBindingKey() const
{
	return static_cast<bool>(keyBindCallback_);
}

const char* OverlayKeyboardInput::GetKeyName(DWORD vkCode)
{
	if (vkCode >= 'A' && vkCode <= 'Z')
	{
		static thread_local char name[2] = {};
		name[0] = static_cast<char>(vkCode);
		return name;
	}

	if (vkCode >= '0' && vkCode <= '9')
	{
		static thread_local char name[2] = {};
		name[0] = static_cast<char>(vkCode);
		return name;
	}

	switch (vkCode)
	{
	case 0: return "None";
	case VK_LBUTTON: return "Mouse Left";
	case VK_RBUTTON: return "Mouse Right";
	case VK_MBUTTON: return "Mouse Middle";
	case VK_XBUTTON1: return "Mouse X1";
	case VK_XBUTTON2: return "Mouse X2";
	case VK_LSHIFT: return "Left Shift";
	case VK_LCONTROL: return "Left Ctrl";
	case VK_OEM_MINUS: return "-";
	case VK_SUBTRACT: return "Numpad -";
	case VK_SPACE: return "Space";
	case VK_RETURN: return "Enter";
	case VK_NUMPAD0: return "Numpad 0";
	case VK_NUMPAD1: return "Numpad 1";
	case VK_NUMPAD2: return "Numpad 2";
	case VK_NUMPAD3: return "Numpad 3";
	case VK_NUMPAD4: return "Numpad 4";
	case VK_NUMPAD5: return "Numpad 5";
	case VK_NUMPAD6: return "Numpad 6";
	case VK_NUMPAD7: return "Numpad 7";
	case VK_NUMPAD8: return "Numpad 8";
	case VK_NUMPAD9: return "Numpad 9";
	default: return "Unknown";
	}
}

void OverlayKeyboardInput::KeyBind(DWORD& targetKey, bool& isWaiting)
{
	if (!isWaiting)
	{
		CancelKeyBind();
		return;
	}

	if (IsBindingKey())
		return;

	BeginKeyBind([&targetKey, &isWaiting](DWORD vkCode) {
		targetKey = vkCode;
		isWaiting = false;
		});
}

LRESULT OverlayKeyboardInput::HandleKeyboard(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(keyboardHook_, nCode, wParam, lParam);

	const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
	const bool isKeyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
	const bool isKeyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

	if (isKeyUp && capturedBindKey_ == key.vkCode)
	{
		capturedBindKey_ = 0;
		return CallNextHookEx(keyboardHook_, nCode, wParam, lParam);
	}

	if (isKeyDown && IsBindingKey())
	{
		HandleKeyBind(key.vkCode);
		return CallNextHookEx(keyboardHook_, nCode, wParam, lParam);
	}

	ForwardToImGui(key, wParam);
	return CallNextHookEx(keyboardHook_, nCode, wParam, lParam);
}

LRESULT OverlayKeyboardInput::HandleMouse(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(mouseHook_, nCode, wParam, lParam);

	const auto& mouse = *reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
	const DWORD vkCode = MouseMessageToVirtualKey(wParam, mouse);
	if (vkCode != 0 && IsBindingKey())
	{
		HandleKeyBind(vkCode);
		return CallNextHookEx(mouseHook_, nCode, wParam, lParam);
	}

	return CallNextHookEx(mouseHook_, nCode, wParam, lParam);
}

bool OverlayKeyboardInput::HandleKeyBind(DWORD vkCode)
{
	if (!keyBindCallback_)
		return false;

	if (!IsAllowedKeyBindVirtualKey(vkCode))
		return false;

	capturedBindKey_ = vkCode;
	auto callback = std::move(keyBindCallback_);
	keyBindCallback_ = nullptr;
	callback(vkCode == VK_ESCAPE ? 0 : vkCode);
	return true;
}

void OverlayKeyboardInput::ForwardModifierToImGui(DWORD vkCode, bool isKeyDown)
{
	switch (vkCode)
	{
	case VK_SHIFT:
	case VK_LSHIFT:
		leftShiftDown_ = isKeyDown;
		break;
	case VK_RSHIFT:
		rightShiftDown_ = isKeyDown;
		break;
	case VK_CONTROL:
	case VK_LCONTROL:
		leftCtrlDown_ = isKeyDown;
		break;
	case VK_RCONTROL:
		rightCtrlDown_ = isKeyDown;
		break;
	case VK_MENU:
	case VK_LMENU:
		leftAltDown_ = isKeyDown;
		break;
	case VK_RMENU:
		rightAltDown_ = isKeyDown;
		break;
	case VK_LWIN:
		leftSuperDown_ = isKeyDown;
		break;
	case VK_RWIN:
		rightSuperDown_ = isKeyDown;
		break;
	default:
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiMod_Ctrl, leftCtrlDown_ || rightCtrlDown_);
	io.AddKeyEvent(ImGuiMod_Shift, leftShiftDown_ || rightShiftDown_);
	io.AddKeyEvent(ImGuiMod_Alt, leftAltDown_ || rightAltDown_);
	io.AddKeyEvent(ImGuiMod_Super, leftSuperDown_ || rightSuperDown_);
}

void OverlayKeyboardInput::ForwardToImGui(const KBDLLHOOKSTRUCT& key, WPARAM message)
{
	ImGuiIO& io = ImGui::GetIO();
	const bool isKeyDown = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
	const bool isKeyUp = message == WM_KEYUP || message == WM_SYSKEYUP;

	if (!isKeyDown && !isKeyUp)
		return;

	if (IsModifierVirtualKey(key.vkCode))
	{
		ForwardModifierToImGui(key.vkCode, isKeyDown);
		return;
	}

	const bool isShortcutModifierDown = leftCtrlDown_
		|| rightCtrlDown_
		|| leftAltDown_
		|| rightAltDown_
		|| leftSuperDown_
		|| rightSuperDown_
		|| IsShortcutModifierDown();
	if (isKeyDown && isShortcutModifierDown)
		return;

	const ImGuiKey imguiKey = ToImGuiKey(key.vkCode);
	if (imguiKey != ImGuiKey_None)
		io.AddKeyEvent(imguiKey, isKeyDown);

	if (isKeyDown && IsCharacterVirtualKey(key.vkCode))
		AddInputCharacter(key);
}

LRESULT CALLBACK OverlayKeyboardInput::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!instance_)
		return CallNextHookEx(nullptr, nCode, wParam, lParam);

	return instance_->HandleKeyboard(nCode, wParam, lParam);
}

LRESULT CALLBACK OverlayKeyboardInput::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!instance_)
		return CallNextHookEx(nullptr, nCode, wParam, lParam);

	return instance_->HandleMouse(nCode, wParam, lParam);
}

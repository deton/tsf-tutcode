#include "common.h"
#include "KeySender.h"
#include "mozc/win32/base/keyboard.h"
#include <memory>

static bool isextendedkey(UINT vk)
{
	// http://stackoverflow.com/questions/9233679/sendinput-doesnt-send-chars-or-numbers
	return vk >= VK_PRIOR && vk <= VK_DELETE || vk >= VK_LWIN && vk <= VK_APPS;
}

static void _QueueKey(std::vector<INPUT> *inputs, UINT vk, int count = 1)
{
	const KEYBDINPUT keyboard_input = {vk, 0, 0, 0, 0};
	INPUT keydown = {};
	keydown.type = INPUT_KEYBOARD;
	keydown.ki = keyboard_input;

	INPUT keyup = keydown;
	keyup.type = INPUT_KEYBOARD;
	keyup.ki.dwFlags = KEYEVENTF_KEYUP;

	if (isextendedkey(vk))
	{
		keydown.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		keyup.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}

	for (int i = 0; i < count; ++i)
	{
		inputs->push_back(keydown);
		inputs->push_back(keyup);
	}
}

static void _QueueKeyForModifier(std::vector<INPUT> *inputs, UINT vk, BOOL up, BOOL front = FALSE)
{
	const KEYBDINPUT keyboard_input = {vk, 0, 0, 0, 0};
	INPUT keydown = {};
	keydown.type = INPUT_KEYBOARD;
	keydown.ki = keyboard_input;

	if (up)
	{
		INPUT keyup = keydown;
		keyup.type = INPUT_KEYBOARD;
		keyup.ki.dwFlags = KEYEVENTF_KEYUP;
		if (front)
		{
			inputs->insert(inputs->begin(), keyup);
		}
		else
		{
			inputs->push_back(keyup);
		}
	}
	else
	{
		if (front)
		{
			inputs->insert(inputs->begin(), keydown);
		}
		else
		{
			inputs->push_back(keydown);
		}
	}
}

static void _SendInputs(std::vector<INPUT> *inputs)
{
	std::unique_ptr<mozc::win32::Win32KeyboardInterface> keyboard_(
			mozc::win32::Win32KeyboardInterface::CreateDefault());
	// cf. deleter.UnsetModifiers()
	mozc::win32::KeyboardStatus keyboard_state;
	bool shiftPressed = false;
	bool controlPressed = false;
	if (keyboard_->GetKeyboardState(&keyboard_state))
	{
		const BYTE kUnsetState = 0;
		bool to_be_updated = false;
		if (keyboard_state.IsPressed(VK_SHIFT))
		{
			shiftPressed = true;
			to_be_updated = true;
			keyboard_state.SetState(VK_SHIFT, kUnsetState);
			_QueueKeyForModifier(inputs, VK_SHIFT, TRUE, TRUE);
			// restore modifier
			_QueueKeyForModifier(inputs, VK_SHIFT, FALSE);
		}
		if (keyboard_state.IsPressed(VK_CONTROL))
		{
			controlPressed = true;
			to_be_updated = true;
			keyboard_state.SetState(VK_CONTROL, kUnsetState);
			_QueueKeyForModifier(inputs, VK_CONTROL, TRUE, TRUE);
			_QueueKeyForModifier(inputs, VK_CONTROL, FALSE);
		}
		if (to_be_updated)
		{
			keyboard_->SetKeyboardState(keyboard_state);
		}
	}

	keyboard_->SendInput(*inputs);
}

HRESULT KeySender::OtherIme()
{
	std::vector<INPUT> inputs;

	if (IsWindowsVersion62OrLater()) // Win+Space (Windows 8)
	{
		_QueueKeyForModifier(&inputs, VK_LWIN, FALSE);
		_QueueKey(&inputs, VK_SPACE);
		_QueueKeyForModifier(&inputs, VK_LWIN, TRUE);
	}
	else // Alt+Shift for Windows 7
	{
		_QueueKeyForModifier(&inputs, VK_MENU, FALSE);
		_QueueKey(&inputs, VK_SHIFT);
		_QueueKeyForModifier(&inputs, VK_MENU, TRUE);
	}
	_SendInputs(&inputs);
	return S_OK;
}

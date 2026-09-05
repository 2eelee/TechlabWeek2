#pragma once

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <bitset>
#include "FIntPoint.h"

enum MouseButton
{
	LEFT = 0x01,
	RIGHT = 0x02
};
class InputManager
{
public:
	static InputManager& GetInstance();
	void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();
	bool GetKeyDown(WPARAM wParam);
	bool GetKey(WPARAM wParam);
	bool GetKeyUp(WPARAM wParam);

	bool GetMouseButtonDown(MouseButton type);
	bool GetMouseButton(MouseButton type);
	bool GetMouseButtonUp(MouseButton type);

	FIntPoint GetMousePosition();

private:
	InputManager() = default;
	~InputManager() = default;
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	std::bitset<256> m_current{};
	std::bitset<256> m_prev{};

	FIntPoint m_mousePosition{};
};
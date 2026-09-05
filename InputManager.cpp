#include "InputManager.h"

InputManager& InputManager::GetInstance()
{
	static InputManager instance;
	return instance;
}

void InputManager::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_current[wParam] = true;
		break;
	case WM_KEYUP:
		m_current[wParam] = false;
		break;
	case WM_LBUTTONUP:
		m_current[MouseButton::LEFT] = false;
		break;
	case WM_RBUTTONUP:
		m_current[MouseButton::RIGHT] = false;
		break;
	case WM_MOUSEMOVE:
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		m_mousePosition.x = xPos;
		m_mousePosition.y = yPos;
	}
}

void InputManager::Update()
{
	m_prev = m_current;
}

bool InputManager::GetKeyDown(WPARAM wParam)
{
	return m_prev[wParam] == false && m_current[wParam] == true;
}

bool InputManager::GetKey(WPARAM wParam)
{
	return m_prev[wParam] == true && m_current[wParam] == true;
}

bool InputManager::GetKeyUp(WPARAM wParam)
{
	return m_prev[wParam] == true && m_current[wParam] == false;
}

bool InputManager::GetMouseButtonDown(MouseButton type)
{
	return m_prev[type] == false && m_current[type] == true;
}

bool InputManager::GetMouseButton(MouseButton type)
{
	return m_prev[type] == true && m_current[type] == true;
}

bool InputManager::GetMouseButtonUp(MouseButton type)
{
	return m_prev[type] == true && m_current[type] == false;
}

FIntPoint InputManager::GetMousePosition()
{
	return m_mousePosition;
}
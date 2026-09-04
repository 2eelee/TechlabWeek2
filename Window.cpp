#include "Window.h"
#include "URenderer.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"

URenderer* GRenderer = nullptr;

UINT GWindowWidth = 1024; //호스트 창의 너비 사이즈 
UINT GWindowHeight = 1024; //호스트 창의 높이 사이즈
bool GWindowSizeChanged = false; // 호스트 창의 사이즈가 변경되었는지 여부를 나타내는 플래그

bool GIsResizing = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_ENTERSIZEMOVE:
	{
		GIsResizing = true;
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		GIsResizing = false;
		break;
	}
	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			GWindowWidth = width;
			GWindowHeight = height; //현재 호스트 창 크기 저장

			GWindowSizeChanged = true;// 호스트 창의 크기가 변경되면 false에서 true로 변경

			if (GRenderer != nullptr)
			{
				GRenderer->Resize(width, height);
			}


		}
		break;
			
	}

	case WM_DESTROY:
		// Signal that the app should quit
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HWND FWindow::Create(HINSTANCE hInstance)
{
	// 윈도우 클래스 이름
	WCHAR WindowClass[] = L"JungleWIndowClass";

	// 윈도우 타이틀바에 표시될 이름
	WCHAR Title[] = L"Game Tech Lab";

	// 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다. 
	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

	// 윈도우 클래스 등록
	RegisterClassW(&wndclass);

	// 1024 x 1024 크기의 윈도우 생성
	HWND hWnd = CreateWindowExW(
		0, WindowClass, Title,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
		nullptr, nullptr, hInstance, nullptr
	);

	return hWnd;
}
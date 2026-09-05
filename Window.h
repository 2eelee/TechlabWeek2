#pragma once

#include <Windows.h>

class FWindow
{
public:
	HWND Create(HINSTANCE hInstance);
};

// 다른 cpp 파일에서 사용할 수 있도록 GWindowSizeChanged, GWindowWidth, GWindowHeight, GIsResizing를 extern으로 선언
extern bool GWindowSizeChanged;
extern UINT GWindowWidth;
extern UINT GWindowHeight;
extern bool GIsResizing;
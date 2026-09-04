#pragma once

#include <Windows.h>
#include <d3d11.h>

class FImGuiManager
{
public:
	void Create(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void BeginFrame();
	void EndFrame();
	void Release();
};
#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>

#include "FConsoleWindow.h"

#include "FVector3.h"
#include "FVertexSimple.h"
#include "URenderer.h"

#include "Window.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"

#include "Sphere.h"

// WinMain
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstanc ,LPSTR lpCmdLine,int nCmdShow)
{
	// Window 생성
	

	FWindow window;

	HWND hWnd =window.Create(hInstance);

	extern URenderer* GRenderer;

	// Renderer 생성
	
	URenderer renderer;


	renderer.Create(hWnd);

	// Window.cpp에서 사용하는 Renderer와 연결
	GRenderer = &renderer;

	// Shader 생성
	renderer.CreateShader();

	// Constant Buffer 생성
	renderer.CreateConstantBuffer();

	// ImGui 생성
	
	FImGuiManager imguiManager;

	imguiManager.Create(hWnd,renderer.Device,renderer.DeviceContext);
	// Sphere Vertex Buffer

	UINT numVerticesSphere = sizeof(sphere_vertices) /sizeof(FVertexSimple);

	ID3D11Buffer*
		vertexBufferSphere =
		renderer.CreateVertexBuffer( sphere_vertices, sizeof(sphere_vertices));

	// Console 객체
	
	// while 밖에 있어야 로그가 계속 유지됨

	FConsoleWindow console;

	bool showConsole = true;


	// Console / Host 비율
	// 처음에는
		// Width  = Host의 50%
	// Height = Host의 30%

	float consoleWidthRatio =0.5f;

	float consoleHeightRatio = 0.3f;

	bool consoleRatioInitialized = false;

	// 프로그램 종료 여부
	
	bool bIsExit = false;

	// FPS

	const int targetFPS = 30;

	const double targetFrameTime = 1000.0 /targetFPS;

	// Timer

	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;

	double elapsedTime =
		0.0;
	// Main Loop


	while (bIsExit == false)
	{
		// Frame 시작 시간

	QueryPerformanceCounter(&startTime);
		// Windows Message
		MSG msg;

		while (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				bIsExit = true;

				break;
			}
		}


		if (bIsExit)
		{
			break;
		}


		// Renderer 준비
	

		renderer.Prepare();

		renderer.PrepareShader();


		// ImGui Frame 시작


		imguiManager.BeginFrame();

		// 현재 Host 크기
		// 반드시 while 안에서 매 프레임 다시 가져옴


		ImGuiIO& io = ImGui::GetIO();


		float hostWidth = io.DisplaySize.x;


		float hostHeight = io.DisplaySize.y;

		// Console 첫 크기 설정
		// 처음 한 번:
		
		// Host × 0.5
		// Host × 0.3
	

		if (!consoleRatioInitialized)
		{
			ImGui::SetNextWindowSize(ImVec2( hostWidth *consoleWidthRatio, hostHeight *consoleHeightRatio), ImGuiCond_Always);
		}



		// Host 크기가 변경됐다면
		//
		// 직전에 저장되어 있던 Console 비율을 이용해서
		// Console 크기도 같이 변경
		

		else if (
			GWindowSizeChanged)
		{
			float newConsoleWidth = hostWidth *consoleWidthRatio;


			float newConsoleHeight = hostHeight *consoleHeightRatio;



			ImGui::SetNextWindowSize(ImVec2(newConsoleWidth,newConsoleHeight),ImGuiCond_Always);
		}



	
		// Console Draw Begin() 로그 출력 Input Enter End() 전부 여기 안에서 처리
	

		if (showConsole)
		{
			console.Draw("Example: Console", &showConsole);
		}

		
		// Host 자체를 Resize하고 있는 중이 아니라면
		// 사용자가 Console을 바꾼 결과를 계속 저장
	

		if (!GIsResizing &&showConsole)
		{
			if (hostWidth > 0.0f &&hostHeight > 0.0f)
			{
				// 현재 Console Width / Host Width
		

				consoleWidthRatio = console.WindowSize.x / hostWidth;



				// 현재 Console Height / Host Height
				consoleHeightRatio =console.WindowSize.y /hostHeight;
				consoleRatioInitialized =true;
			}
		}

	// 이번 Host Resize 처리는 끝났음
		GWindowSizeChanged =false;

		// ImGui Frame 종료
		imguiManager.EndFrame();

		// Buffer 교환
		renderer.SwapBuffer();

		// FPS 제한
		
		do
		{
			Sleep(0);
			QueryPerformanceCounter(&endTime);


			elapsedTime =(endTime.QuadPart -startTime.QuadPart)*1000.0 /frequency.QuadPart;


		} while (elapsedTime <targetFrameTime);
	}
	// ImGui 소멸
	imguiManager.Release();

	// Vertex Buffer 소멸

	renderer.ReleaseVertexBuffer(vertexBufferSphere);
	// Constant Buffer 소멸
	renderer.ReleaseConstantBuffer();

	// Shader 소멸
	renderer.ReleaseShader();
	// Renderer 소멸
	renderer.Release();
	return 0;
}
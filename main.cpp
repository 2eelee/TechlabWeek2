#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>

#include "FVector3.h"
#include "FVertexSimple.h"
#include "URenderer.h"

#include "Window.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"

#include "Sphere.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FWindow window;
	HWND hWnd = window.Create(hInstance);

	extern URenderer* GRenderer;

	// Renderer Class를 생성합니다.
	URenderer renderer;

	// 초기화
	//renderer.Initialize( );


	// D3D11 생성하는 함수를 호출합니다. 
	renderer.Create(hWnd);

	// 같은 Renderer 연결
	GRenderer = &renderer;


	// 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
	renderer.CreateShader();

	// 여기에 생성 함수를 추가합니다. 
	renderer.CreateConstantBuffer();

	FImGuiManager imguiManager;

	// 여기에서 ImGui를 생성합니다.
	imguiManager.Create(hWnd, renderer.Device, renderer.DeviceContext);

	UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);

	ID3D11Buffer* vertexBufferSphere =
		renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

	// 제어용 변수
	bool bIsExit = false;

	// FPS 제한을 위한 설정
	const int targetFPS = 30;
	const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

	// 고성능 타이머 초기화
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;



	// 콘솔 창의 크기를 화면 비율로 설정
	float consoleWidthRatio = 0.5f;
	float consoleHeightRatio = 0.3f;

	// Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨
	while (bIsExit == false)
	{
		// 루프 시작 시간 기록
		QueryPerformanceCounter(&startTime);

		MSG msg;

		// Main message loop
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}
		}

		////////////////////////////////////////////
		// 매번 실행되는 코드를 여기에 추가합니다.

		// 준비 작업
		renderer.Prepare();
		renderer.PrepareShader();

		imguiManager.BeginFrame();

		ImGuiIO& io = ImGui::GetIO();

		// 호스트 창 자체가 Resize 됐다면
        // 저장된 비율로 Console 조정
  

		if (GWindowSizeChanged)
		{
		

			float newWidth =
				io.DisplaySize.x * consoleWidthRatio;

			float newHeight =
				io.DisplaySize.y * consoleHeightRatio;

			/*ImGui::SetNextWindowPos(
				ImVec2(newX, newY),
				ImGuiCond_Always
			);*/

			ImGui::SetNextWindowSize(
				ImVec2(newWidth, newHeight),
				ImGuiCond_Always
			);
		}

		// 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다. 

		ImGui::Begin("Jungle Property Window");

		ImGui::Text("Hello Jungle World!");

		// 현재 위치
		ImVec2 consolePos =
			ImGui::GetWindowPos();

		// 현재 크기
		ImVec2 consoleSize =
			ImGui::GetWindowSize();


		// 호스트 자체 Resize 중이 아니라면
		// 사용자가 직접 만든 위치/크기를 비율로 저장
		if (!GWindowSizeChanged)
		{
			if (io.DisplaySize.x > 0 &&
				io.DisplaySize.y > 0)
			{
			

				consoleWidthRatio =
					consoleSize.x / io.DisplaySize.x;

				consoleHeightRatio =
					consoleSize.y / io.DisplaySize.y;
			}
		}


		ImGui::End();

		imguiManager.EndFrame();

		// 현재 화면에 보여지는 버퍼와 그리기 작업을 위한 버퍼를 서로 교환합니다.
		renderer.SwapBuffer();

		do
		{
			Sleep(0);

			// 루프 종료 시간 기록
			QueryPerformanceCounter(&endTime);

			//한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
			elapsedTime =
				(endTime.QuadPart - startTime.QuadPart)
				* 1000.0 / frequency.QuadPart;

		} while (elapsedTime < targetFrameTime);

		////////////////////////////////////////////
	}

	// 여기에서 ImGui 소멸
	imguiManager.Release();

	// 버텍스 버퍼 소멸은 Renderer 소멸 전에 처리합니다.
	renderer.ReleaseVertexBuffer(vertexBufferSphere);

	// ReleaseShader() 직전에 소멸 함수를 추가합니다.
	renderer.ReleaseConstantBuffer();

	renderer.ReleaseShader();

	// D3D11 소멸 시키는 함수를 호출합니다.
	renderer.Release();

	return 0;
}
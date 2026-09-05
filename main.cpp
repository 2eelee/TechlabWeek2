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
#include "FMemory.h"
#include "MeshManager.h"
#include "Vertices.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "FObjectFactory.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FWindow window;
	HWND hWnd = window.Create(hInstance);

	// Renderer Class를 생성합니다.
	URenderer renderer;

	// D3D11 생성하는 함수를 호출합니다. 
	renderer.Create(hWnd);

	// 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
	renderer.CreateShader();

	// 여기에 생성 함수를 추가합니다.
	MeshManager::Get().Initialize(renderer);
	renderer.CreateConstantBuffer();

	FImGuiManager imguiManager;

	// 여기에서 ImGui를 생성합니다.
	imguiManager.Create(hWnd, renderer.Device, renderer.DeviceContext);

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

	std::size_t BeforeMemory = FMemory::GetCurrentMemoryUsage();
	uint64 BeforeCount = FMemory::GetAllocationCount();


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

		// 테스트용 MVP 코드
		FMatrix Model = FMatrix::CreateTranslation(0.0f, 0.0f, 0.0f);
		
		FMatrix View = FMatrix::CreateView(
			FVector3(0.0f, 0.0f, -3.0f), // 위치 (Eye)
			FVector3(1.0f, 0.0f, 0.0f),  // Right (X축)
			FVector3(0.0f, 1.0f, 0.0f),  // Up (Y축)
			FVector3(0.0f, 0.0f, 1.0f)   // Forward (Z축)
		);
		
		float width = renderer.ViewportInfo.Width;
		float height = (renderer.ViewportInfo.Height > 0.0f) ? renderer.ViewportInfo.Height : 1.0f; // 0나누기 방어!
		float aspect = width / height;
		
		FMatrix Proj = FMatrix::CreateProjection(1000.0f, 0.1f, 3.14159265f / 3.0f, aspect);
		
		FMatrix MVP = Model * View * Proj;

		// M * V * P 행렬 입력 
		renderer.UpdateConstant(MVP);
		for (UObject* object : GUObjectArray)
		{
			UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
			if (primitive)
			{
				primitive->Render(renderer);
			}
		}

		imguiManager.BeginFrame();

		// 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다. 

		ImGui::Begin("Stat");

		ImGui::Text("Memory Usage: %zu bytes", FMemory::GetCurrentMemoryUsage());
		ImGui::Text("Allocation Count: %llu", static_cast<unsigned long long>(FMemory::GetAllocationCount()));

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
	MeshManager::Get().Release(renderer);

	// ReleaseShader() 직전에 소멸 함수를 추가합니다.
	renderer.ReleaseConstantBuffer();

	renderer.ReleaseShader();

	// D3D11 소멸 시키는 함수를 호출합니다.
	renderer.Release();

	return 0;
}
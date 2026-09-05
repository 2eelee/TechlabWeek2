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

#include "FRenderer.h"

#include "FObjectFactory.h"
#include "USceneComponent.h"

#include "Sphere.h"
#include "Cube.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FWindow window;
	HWND hWnd = window.Create(hInstance);

	// Renderer Class를 생성합니다.
	URenderer renderer;
	FRenderer FRenderer(renderer);

	// D3D11 생성하는 함수를 호출합니다. 
	renderer.Create(hWnd);

	// 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
	renderer.CreateShader();

	// 여기에 생성 함수를 추가합니다. 
	renderer.CreateConstantBuffer();

	FImGuiManager imguiManager;

	// 여기에서 ImGui를 생성합니다.
	imguiManager.Create(hWnd, renderer.Device, renderer.DeviceContext);

	UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
	UINT numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);

	ID3D11Buffer* vertexBufferSphere =
		renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
	ID3D11Buffer* vertexBufferCube =
		renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));

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


	UCamera camera;
	camera.Location = FVector3(0.0f, 0.0f, -5.0f);
	camera.Rotation = FVector3(0.0f, 0.0f, 0.0f);
	camera.FOV = 3.14159265f / 3.0f;

	TSubclassOf<UObject> cubeClass = UCubeComp::StaticClass();
	UObject* cubeObject = FObjectFactory::ConstructObject(cubeClass);
	UCubeComp* cube = UObject::Cast<UCubeComp>(cubeObject);

	TSubclassOf<UObject> sphereClass = USphereComp::StaticClass();
	UObject* sphereObject = FObjectFactory::ConstructObject(sphereClass);
	USphereComp* sphere = UObject::Cast<USphereComp>(sphereObject);

	cube->RelativeLocation = FVector3(0.5f, 0.2f, 0.0f);
	cube->RelativeRotation = FVector3(0.3f, 0.7f, 0.2f); // rad
	cube->RelativeScale3D = FVector3(1.5f, 0.8f, 1.2f);

	sphere->RelativeLocation = FVector3(-0.8f, -0.25f, 0.4f);
	sphere->RelativeRotation = FVector3(0.2f, -0.5f, 0.35f);
	sphere->RelativeScale3D = FVector3(0.7f, 1.1f, 0.7f);

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

		// 렌더링 준비 작업
		renderer.Prepare();
		renderer.PrepareShader();

		FRenderer.Render(
			camera,
			*cube,
			vertexBufferCube,
			numVerticesCube
		);

		FRenderer.Render(
			camera,
			*sphere,
			vertexBufferSphere,
			numVerticesSphere
		);

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
	renderer.ReleaseVertexBuffer(vertexBufferSphere);

	// ReleaseShader() 직전에 소멸 함수를 추가합니다.
	renderer.ReleaseConstantBuffer();

	renderer.ReleaseShader();

	// D3D11 소멸 시키는 함수를 호출합니다.
	renderer.Release();

	return 0;
}
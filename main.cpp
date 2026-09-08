#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>

#include "FConsoleWindow.h"

#include "URenderer.h"
#include "UCamera.h"
#include "FGizmo.h"

#include "Window.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"
#include "FMemory.h"

#include "MeshManager.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "FObjectFactory.h"
#include "FEditor.h"
#include "FMousePicker.h"
#include "InputManager.h"

FConsoleWindow* GConsoleWindow = nullptr;

// WinMain
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstanc ,LPSTR lpCmdLine,int nCmdShow)
{
	// Window 생성
	FWindow window;

	HWND hWnd =window.Create(hInstance);

	// 1. 카메라 생성
	UCamera* camera = new UCamera();
	camera->SetRelativeLocation(FVector3(0.0f, 5.0f, -10.0f));
	camera->FovAngle = 80.0f;
	camera->AddPitch(20.0f);

	FEditor EditorUI;
	FMousePicker MousePicker;
	GMousePicker = &MousePicker;

	FConsoleWindow console;
	extern FConsoleWindow* GConsoleWindow;
	GConsoleWindow = &console;
	extern URenderer* GRenderer;

	// Renderer 생성
	URenderer renderer;
	renderer.Create(hWnd);

	// Window.cpp에서 사용하는 Renderer와 연결
	GRenderer = &renderer;

	// Shader 생성
	renderer.CreateShader();

	// BlendState 생성
	renderer.CreateAlphaBlendState();

	// 여기에 생성 함수를 추가합니다.
	MeshManager::Get().Initialize(renderer);
	renderer.CreateConstantBuffer();

	FGizmo Gizmo;
	Gizmo.Initialize(renderer);

	// ImGui 생성
	FImGuiManager imguiManager;

	// 여기에서 ImGui를 생성합니다.
	imguiManager.Create(hWnd, renderer.Device, renderer.DeviceContext);
	ImGuiIO& io = ImGui::GetIO();

	// Console / Host 비율
	// 처음에는
	// Width  = Host의 50%
	// Height = Host의 30%
	bool bIsExit = false;

	// FPS

	const int targetFPS = 30;

	const double targetFrameTime = 1000.0 /targetFPS;

	// Timer
	LARGE_INTEGER frequency{};
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER previousTime{};
	QueryPerformanceCounter(&previousTime);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 0.0;

	// Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨
	while (bIsExit == false)
	{
		// Frame 시작 시간
		QueryPerformanceCounter(&startTime);

		float deltaTime = static_cast<float>(startTime.QuadPart - previousTime.QuadPart) / static_cast<float>(frequency.QuadPart);
		previousTime = startTime;

		// Windows Message
		MSG msg;

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

		if (InputManager::GetInstance().GetKeyDown(VK_SPACE))
		{
			Gizmo.CycleMode();
		}

		if (bIsExit)
		{
			break;
		}

		////////////////////////////////////////////
		// 매번 실행되는 코드를 여기에 추가합니다.
	
		// 준비 작업
		renderer.Prepare();
		renderer.PrepareShader();

		bool allowWorldInput = !ImGui::GetIO().WantCaptureMouse;

		if (allowWorldInput) {
			GMousePicker->HitTestPrimitive(InputManager::GetInstance().GetMousePosition(), *camera, GWindowWidth, GWindowHeight);
			GMousePicker->HitTestGizmoAxis(InputManager::GetInstance().GetMousePosition(), *camera, GWindowWidth, GWindowHeight, Gizmo);
		}
		else {
			GMousePicker->ClearHover();
		}

		GMousePicker->HandleMouseInput(allowWorldInput);
		
		UPrimitiveComponent* closest = MousePicker.GetClosestPrimitive();
		UPrimitiveComponent* selected = MousePicker.GetSelectedPrimitive();

		EGizmoAxis hoveredAxis = MousePicker.GetHoveredAxis();
		EGizmoAxis activeAxis = MousePicker.GetActiveAxis();

		// M * V * P 행렬 입력
		for (UObject* object : GUObjectArray)
		{
			UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
			
			if (primitive)
			{
				FMatrix MVP = renderer.CreateMVP(*primitive, camera);
				bool IsHighlighted = (closest && (closest->UUID == object->UUID)) || (selected && (selected->UUID == object ->UUID));
				renderer.UpdateConstant(MVP, IsHighlighted);
				primitive->Render(renderer);
			}
		}
		
		Gizmo.DrawGrid(renderer, camera);

		if (selected)
		{
			Gizmo.DrawLocalAxis(renderer, camera, selected);
			Gizmo.DrawTransformGizmo(renderer, camera, selected, hoveredAxis);
		}

		Gizmo.DrawWorldAxis(renderer, camera);
		camera->CamMove(deltaTime);

		// ImGui Frame 시작
		imguiManager.BeginFrame();
		
		// 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다. 
		EditorUI.UpdateWindowSize();
		EditorUI.DrawConsoleUI();
		EditorUI.DrawPropertyUI();
		EditorUI.DrawControlUI();
		EditorUI.DrawStatUI();

		GWindowSizeChanged = false;

		imguiManager.EndFrame();

		InputManager::GetInstance().Update();

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

	Gizmo.Release(renderer);

	// 버텍스 버퍼 소멸은 Renderer 소멸 전에 처리합니다.
	MeshManager::Get().Release(renderer);

	// ReleaseShader() 직전에 소멸 함수를 추가합니다.
	// Vertex Buffer 소멸

	// Constant Buffer 소멸
	renderer.ReleaseConstantBuffer();

	// Shader 소멸
	renderer.ReleaseShader();

	// BlendState 소멸
	renderer.ReleaseAlphaBlendState();

	// Renderer 소멸
	renderer.Release();
	return 0;
}
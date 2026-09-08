#pragma once

#include <d3dcompiler.h>
#include <Windows.h>
#include <d3d11.h>
#include "FVector3.h"
#include "FVertexSimple.h"
#include "FConstants.h"
#include "FMatrix.h"

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")



class UPrimitiveComponent;
class UCamera;

class URenderer
{
public:
	float AspectRatio;
	FMatrix CreateMVP(UPrimitiveComponent& Primitive, UCamera* Camera);
	FMatrix CreateMVPFromModel(const FMatrix& Model, UCamera* Camera);
	// Direct3D 11 장치와 장치 컨텍스트 및 스왑 체인을 관리하기 위한 포인터들
	ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
	ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr; // 화면 출력용 텍스처
	ID3D11RenderTargetView* FrameBufferRTV = nullptr; // 텍스처를 렌더 타겟으로 사용하는 뷰
	ID3D11RasterizerState* RasterizerState = nullptr; // 래스터라이저 상태(컬링, 채우기 모드 등 정의)
	ID3D11RasterizerState* NoCullRasterizerState = nullptr;

	ID3D11BlendState* AlphaBlendState = nullptr;

	ID3D11Texture2D* DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthReadOnlyState = nullptr;


	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상(RGBA)
	D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

	ID3D11VertexShader* SimpleVertexShader = nullptr;
	ID3D11PixelShader* SimplePixelShader = nullptr;
	ID3D11InputLayout* SimpleInputLayout = nullptr;

	ID3D11Buffer* ConstantBuffer = nullptr;

	unsigned int Stride = 0;


	void Create(HWND hWindow);

	void CreateDeviceAndSwapChain(HWND hWindow);

	// Direct3D 장치 및 스왑 체인을 해제하는 함수
	void ReleaseDeviceAndSwapChain();

	// 프레임 버퍼를 생성하는 함수
	void CreateFrameBuffer();

	// 프레임 버퍼를 해제하는 함수
	void ReleaseFrameBuffer();

	// 래스터라이저 상태를 생성하는 함수
	void CreateRasterizerState();

	void SetCullMode(D3D11_CULL_MODE Mode);

	void Resize(UINT width, UINT height);

	// 래스터라이저 상태를 해제하는 함수
	void ReleaseRasterizerState();

	// 렌더러에 사용된 모든 리소스를 해제하는 함수
	void Release();

	// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
	void SwapBuffer();

	
	void CreateShader();
	void ReleaseShader();

	void Prepare();
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	void PrepareShader();
	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices, UINT startVertexLocation = 0);

	ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);
	void ReleaseVertexBuffer(ID3D11Buffer* pBuffer);

	void CreateAlphaBlendState();
	void SetAlphaBlend(bool bEnable);
	void ReleaseAlphaBlendState();

	void CreateDepthStencilBuffer();
	void CreateDepthStencilState();
	void CreateDepthReadOnlyState();
	void ReleaseDepthStencilBuffer();
	void ReleaseDepthStencilState();
	void SetDepthWrite(bool bEnable);

	void CreateConstantBuffer();
	void ReleaseConstantBuffer();
	void UpdateConstant(const FMatrix& MVP, const bool IsHighlighted = false);
};
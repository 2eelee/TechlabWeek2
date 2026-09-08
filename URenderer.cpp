#include "FMatrix.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "URenderer.h"
#include <iostream>

void URenderer::Create(HWND hWindow)
{
	// Direct3D 장치 및 스왑 체인 생성
	CreateDeviceAndSwapChain(hWindow);

	AspectRatio = ViewportInfo.Width / ViewportInfo.Height;

	// 프레임 버퍼 생성
	CreateFrameBuffer();

	// 래스터라이저 상태 생성
	CreateRasterizerState();

	CreateAlphaBlendState();

	CreateDepthStencilBuffer();
	CreateDepthStencilState();
	CreateDepthReadOnlyState();
}

void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
{
	// 지원하는 Direct3D 기능 레벨을 정의
	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

	// 스왑 체인 설정 구조체 초기화
	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
	swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 색상 포맷
	swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
	swapchaindesc.BufferCount = 2; // 더블 버퍼링
	swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
	swapchaindesc.Windowed = TRUE; // 창 모드
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 효과 설정

	// Direct3D 장치와 스왑 체인을 생성
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
		&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

	// 생성된 스왑 체인의 정보 가져오기
	SwapChain->GetDesc(&swapchaindesc);

	// 뷰포트 정보 설정
	ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
}

// Direct3D 장치 및 스왑 체인을 해제하는 함수
void URenderer::ReleaseDeviceAndSwapChain()
{
	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	if (DeviceContext)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}

	if (Device)
	{
		Device->Release();
		Device = nullptr;
	}
}

// 프레임 버퍼를 생성하는 함수
void URenderer::CreateFrameBuffer()
{
	// 스왑 체인에서 백 버퍼 텍스처 가져오기
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

	// 렌더 타겟 뷰 생성
	D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
	framebufferRTVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 색상 포맷
	framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

	Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
}

// 프레임 버퍼를 해제하는 함수
void URenderer::ReleaseFrameBuffer()
{
	if (FrameBuffer)
	{
		FrameBuffer->Release();
		FrameBuffer = nullptr;
	}

	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
}

// 래스터라이저 상태를 생성하는 함수
void URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드

	rasterizerDesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링
	Device->CreateRasterizerState(&rasterizerDesc, &RasterizerState);

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	Device->CreateRasterizerState(&rasterizerDesc, &NoCullRasterizerState);
}

void URenderer::SetCullMode(D3D11_CULL_MODE Mode)
{
	if (Mode == D3D11_CULL_NONE)
		DeviceContext->RSSetState(NoCullRasterizerState);
	else
		DeviceContext->RSSetState(RasterizerState);
}

void URenderer::Resize(UINT width, UINT height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	ReleaseFrameBuffer();

	DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	ReleaseDepthStencilBuffer();
	ReleaseFrameBuffer();

	SwapChain->ResizeBuffers(
		0,
		width,
		height,
		DXGI_FORMAT_UNKNOWN,
		0
	);

	ViewportInfo.Width = static_cast<float>(width);
	ViewportInfo.Height = static_cast<float>(height);

	AspectRatio = ViewportInfo.Width / ViewportInfo.Height;

	CreateFrameBuffer();
	CreateDepthStencilBuffer();

	DeviceContext->RSSetViewports(1, &ViewportInfo);
}

// 래스터라이저 상태를 해제하는 함수
void URenderer::ReleaseRasterizerState()
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}

	if (NoCullRasterizerState)
	{
		NoCullRasterizerState->Release();
		NoCullRasterizerState = nullptr;
	}
}

// 렌더러에 사용된 모든 리소스를 해제하는 함수
void URenderer::Release()
{
	if (DeviceContext)
	{
		DeviceContext->ClearState();
		DeviceContext->Flush();
	}

	ReleaseAlphaBlendState();
	ReleaseRasterizerState();

	ReleaseDepthStencilState();
	ReleaseDepthStencilBuffer();

	ReleaseFrameBuffer();

	ReleaseDeviceAndSwapChain();
}

// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
void URenderer::SwapBuffer()
{
	SwapChain->Present(1, 0); // 1: VSync 활성화
}

ID3D11VertexShader* SimpleVertexShader = nullptr;
ID3D11PixelShader* SimplePixelShader = nullptr;
ID3D11InputLayout* SimpleInputLayout = nullptr;
unsigned int Stride = 0;

void URenderer::CreateShader()
{
	ID3DBlob* vertexshaderCSO;
	ID3DBlob* pixelshaderCSO;

	D3DCompileFromFile(L"DefaultShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

	Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

	D3DCompileFromFile(L"DefaultShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

	Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

	Stride = sizeof(FVertexSimple);

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
}

void URenderer::ReleaseShader()
{
	if (SimpleInputLayout)
	{
		SimpleInputLayout->Release();
		SimpleInputLayout = nullptr;
	}

	if (SimplePixelShader)
	{
		SimplePixelShader->Release();
		SimplePixelShader = nullptr;
	}

	if (SimpleVertexShader)
	{
		SimpleVertexShader->Release();
		SimpleVertexShader = nullptr;
	}
}

void URenderer::Prepare()
{
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceContext->RSSetViewports(1, &ViewportInfo);
	DeviceContext->RSSetState(RasterizerState);

	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
	DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);
	DeviceContext->OMSetDepthStencilState(DepthReadOnlyState, 0);
}

void URenderer::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	DeviceContext->IASetPrimitiveTopology(Topology);
}

void URenderer::PrepareShader()
{
	DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
	DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
	DeviceContext->IASetInputLayout(SimpleInputLayout);

	if (ConstantBuffer)
	{
		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
		DeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffer);
	}
}

void URenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
{
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

	DeviceContext->Draw(numVertices, 0);
}

ID3D11Buffer* URenderer::CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
{
	// Create a vertex buffer
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

	ID3D11Buffer* vertexBuffer;

	Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

	return vertexBuffer;
}

void URenderer::ReleaseVertexBuffer(ID3D11Buffer* pBuffer)
{
	if (pBuffer)
	{
		pBuffer->Release();
	}
}

void URenderer::CreateAlphaBlendState()
{
	D3D11_BLEND_DESC blendDesc = {};

	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;

	Device->CreateBlendState(&blendDesc, &AlphaBlendState);
}

void URenderer::SetAlphaBlend(bool bEnable)
{
	if (bEnable)
	{
		DeviceContext->OMSetBlendState(
			AlphaBlendState,
			nullptr,
			0xFFFFFFFF
		);
	}
	else
	{
		DeviceContext->OMSetBlendState(
			nullptr,
			nullptr,
			0xFFFFFFFF
		);
	}
}

void URenderer::ReleaseAlphaBlendState()
{
	if (AlphaBlendState)
	{
		AlphaBlendState->Release();
		AlphaBlendState = nullptr;
	}
}

void URenderer::CreateDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthDesc = {};

	depthDesc.Width = static_cast<UINT>(ViewportInfo.Width);
	depthDesc.Height = static_cast<UINT>(ViewportInfo.Height);
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Device->CreateTexture2D(
		&depthDesc,
		nullptr,
		&DepthStencilBuffer
	);

	Device->CreateDepthStencilView(
		DepthStencilBuffer,
		nullptr,
		&DepthStencilView
	);
}

void URenderer::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};

	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDesc.StencilEnable = FALSE;

	Device->CreateDepthStencilState(
		&depthDesc,
		&DepthStencilState
	);
}

void URenderer::CreateDepthReadOnlyState()
{
	D3D11_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.StencilEnable = FALSE;

	Device->CreateDepthStencilState(
		&desc,
		&DepthReadOnlyState
	);
}

void URenderer::ReleaseDepthStencilBuffer()
{
	if (DepthStencilView)
	{
		DepthStencilView->Release();
		DepthStencilView = nullptr;
	}

	if (DepthStencilBuffer)
	{
		DepthStencilBuffer->Release();
		DepthStencilBuffer = nullptr;
	}
}

void URenderer::ReleaseDepthStencilState()
{
	if (DepthStencilState)
	{
		DepthStencilState->Release();
		DepthStencilState = nullptr;
	}

	if (DepthReadOnlyState)
	{
		DepthReadOnlyState->Release();
		DepthReadOnlyState = nullptr;
	}
}

void URenderer::SetDepthWrite(bool bEnable)
{
	DeviceContext->OMSetDepthStencilState(
		bEnable ? DepthStencilState : DepthReadOnlyState,
		0
	);
}

ID3D11Buffer* ConstantBuffer = nullptr;

void URenderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantbufferdesc = {};
	constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
}

void URenderer::ReleaseConstantBuffer()
{
	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
		ConstantBuffer = nullptr;
	}
}

void URenderer::UpdateConstant(const FMatrix& MVP, const bool IsHovered)
{
	if (ConstantBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

		DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant
		FConstants* constants = (FConstants*)constantbufferMSR.pData;
		{
			constants->MVP = MVP;
			constants->IsHovered = IsHovered ? 1 : 0;
		}
		DeviceContext->Unmap(ConstantBuffer, 0);
	}
}

FMatrix URenderer::CreateMVP(UPrimitiveComponent& Primitive, UCamera* Camera)
{
	
	FMatrix Model = Primitive.GetModelMatrix();
	FMatrix scaleM = FMatrix::CreateScale(Primitive.GetRelativeScale3D().x, Primitive.GetRelativeScale3D().y, Primitive.GetRelativeScale3D().z);
	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive.GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(Primitive.GetRelativeRotation().y)) 
		* FMatrix::CreateRotationZ(DegreesToRadians(Primitive.GetRelativeRotation().z));
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive.GetRelativeLocation().x, Primitive.GetRelativeLocation().y, Primitive.GetRelativeLocation().z);

	return CreateMVPFromModel(Model, Camera);
}

FMatrix URenderer::CreateMVPFromModel(const FMatrix& Model, UCamera* Camera)
{
	FMatrix View = FMatrix::CreateView(Camera->GetRelativeLocation(), Camera->GetRightVector(), Camera->GetUPVector(), Camera->GetForwardVector());
	FMatrix Proj;
	if (Camera->othogonalEnable)
	{
		Proj = FMatrix::CreateOrthogonalProjection(Camera->FarZ, Camera->NearZ, Camera->orthowidth, Camera->orthowidth / AspectRatio);
	}
	else Proj = FMatrix::CreateProjection(Camera->FarZ, Camera->NearZ, DegreesToRadians(Camera->FovAngle), AspectRatio);

	return Model * View * Proj;
}
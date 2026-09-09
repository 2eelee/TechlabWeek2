#include "FGizmo.h"
#include "FVertexSimple.h"
#include "URenderer.h"
#include "GizmoVertices.h"
#include "FMousePicker.h"
#include "FMatrix.h"
#include "UCamera.h"

void FGizmo::CycleMode()
{
	switch (CurrentMode)
	{
	case GizmoMode::Translate:
		CurrentMode = GizmoMode::Rotate;
		break;

	case GizmoMode::Rotate:
		CurrentMode = GizmoMode::Scale;
		break;

	case GizmoMode::Scale:
		CurrentMode = GizmoMode::Translate;
		break;
	}
}

const std::vector<FVertexSimple>& FGizmo::GetCurrentVertices() const
{
	if (CurrentMode == GizmoMode::Translate)
	{
		return TranslateVertices;
	}
	else if (CurrentMode == GizmoMode::Rotate)
	{
		return RotateVertices;
	}
	else if (CurrentMode == GizmoMode::Scale)
	{
		return ScaleVertices;
	}
	return TranslateVertices;
}

FMatrix FGizmo::GetTransformGizmoModel(const UPrimitiveComponent* Primitive) const
{
	FMatrix RotationM = Primitive->GetRelativeRotationQuaternion().ToMatrix();
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
	if (CurrentMode == GizmoMode::Translate)
	{
		if (isLocalAxisMode)
			return RotationM * TranslationM;
		return TranslationM;
	}
	else if (CurrentMode == GizmoMode::Rotate)
	{
		if (isLocalAxisMode)
			return RotationM * TranslationM;
		return TranslationM;
	}
	else if (CurrentMode == GizmoMode::Scale)
	{
		return RotationM * TranslationM;
	}
	
	return RotationM * TranslationM;
}

void FGizmo::Initialize(URenderer& Renderer)
{
	LocalAxisVertexBuffer = Renderer.CreateVertexBuffer(LocalAxisVertices, sizeof(LocalAxisVertices));
	WorldAxisVertexBuffer = Renderer.CreateVertexBuffer(WorldAxisVertices, sizeof(WorldAxisVertices));

	auto GridVertices = CreateCheckerGridVertices();

	GridVertexCount = static_cast<UINT>(GridVertices.size());

	GridVertexBuffer = Renderer.CreateVertexBuffer(
		GridVertices.data(),
		static_cast<UINT>(GridVertices.size() * sizeof(FVertexSimple))
	);

	TranslateVertices = CreateTranslateGizmoVertices();
	TranslateGizmoVertexCount = static_cast<UINT>(TranslateVertices.size());
	TranslateGizmoVertexBuffer = Renderer.CreateVertexBuffer(
		TranslateVertices.data(),
		static_cast<UINT>(
			TranslateVertices.size() * sizeof(FVertexSimple)
			)
	);

	RotateVertices = CreateRotateGizmoVertices();
	RotateGizmoVertexCount = static_cast<UINT>(RotateVertices.size());
	RotateGizmoVertexBuffer = Renderer.CreateVertexBuffer(
		RotateVertices.data(),
		static_cast<UINT>(
			RotateVertices.size() * sizeof(FVertexSimple)
			)
	);

	ScaleVertices = CreateScaleGizmoVertices();
	ScaleGizmoVertexCount =	static_cast<UINT>(ScaleVertices.size());
	ScaleGizmoVertexBuffer = Renderer.CreateVertexBuffer(
		ScaleVertices.data(),
		static_cast<UINT>(
			ScaleVertices.size() * sizeof(FVertexSimple)
			)
	);
}

void FGizmo::DrawGrid(URenderer& Renderer, UCamera* Camera)
{
	FMatrix Model = FMatrix::Identity();
	FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
	Renderer.UpdateConstant(MVP);

	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer.SetCullMode(D3D11_CULL_NONE);
	Renderer.SetAlphaBlend(true);
	Renderer.SetDepthWrite(false);

	Renderer.RenderPrimitive(GridVertexBuffer, GridVertexCount);
}

void FGizmo::DrawWorldAxis(URenderer& Renderer, UCamera* Camera)
{
	FMatrix Model = FMatrix::Identity();
	FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);

	Renderer.UpdateConstant(MVP);

	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer.SetAlphaBlend(false);
	Renderer.SetDepthWrite(false);

	Renderer.RenderPrimitive(WorldAxisVertexBuffer, 6);
}

void FGizmo::DrawLocalAxis(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive)
{
	FMatrix RotationM = Primitive->GetRelativeRotationQuaternion().ToMatrix();
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
	FMatrix Model = RotationM * TranslationM;

	FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
	Renderer.UpdateConstant(MVP);

	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer.SetAlphaBlend(false);
	Renderer.SetDepthWrite(false);

	Renderer.RenderPrimitive(LocalAxisVertexBuffer, 6);
}

void FGizmo::DrawTransformGizmo(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive, EGizmoAxis HoveredAxis)
{
	Renderer.SetAlphaBlend(false);
	Renderer.SetDepthWrite(false);
	Renderer.SetCullMode(D3D11_CULL_NONE);
	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FMatrix Model = GetTransformGizmoModel(Primitive);
	FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);

	ID3D11Buffer* gizmoBuffer = nullptr;
	UINT gizmoVertexCount = 0;

	if (CurrentMode == GizmoMode::Translate)
	{
		gizmoBuffer = TranslateGizmoVertexBuffer;
		gizmoVertexCount = TranslateGizmoVertexCount;
	}

	else if (CurrentMode == GizmoMode::Rotate)
	{
		gizmoBuffer = RotateGizmoVertexBuffer;
		gizmoVertexCount = RotateGizmoVertexCount;
	}

	else if (CurrentMode == GizmoMode::Scale)
	{
		gizmoBuffer = ScaleGizmoVertexBuffer;
		gizmoVertexCount = ScaleGizmoVertexCount;
	}

	UINT axisVertexCount = gizmoVertexCount / 3;
	UINT xStart = 0;
	UINT yStart = axisVertexCount;
	UINT zStart = axisVertexCount * 2;

	Renderer.UpdateConstant(MVP, HoveredAxis == EGizmoAxis::X);
	Renderer.RenderPrimitive(gizmoBuffer, axisVertexCount, xStart);

	Renderer.UpdateConstant(MVP, HoveredAxis == EGizmoAxis::Y);
	Renderer.RenderPrimitive(gizmoBuffer, axisVertexCount, yStart);

	Renderer.UpdateConstant(MVP, HoveredAxis == EGizmoAxis::Z);
	Renderer.RenderPrimitive(gizmoBuffer, axisVertexCount, zStart);
}

void FGizmo::Release(URenderer& Renderer)
{
	Renderer.ReleaseVertexBuffer(LocalAxisVertexBuffer);
	Renderer.ReleaseVertexBuffer(WorldAxisVertexBuffer);
	Renderer.ReleaseVertexBuffer(GridVertexBuffer);
	Renderer.ReleaseVertexBuffer(TranslateGizmoVertexBuffer);
	Renderer.ReleaseVertexBuffer(RotateGizmoVertexBuffer);
	Renderer.ReleaseVertexBuffer(ScaleGizmoVertexBuffer);

	LocalAxisVertexBuffer = nullptr;
	WorldAxisVertexBuffer = nullptr;
	GridVertexBuffer = nullptr;
	TranslateGizmoVertexBuffer = nullptr;
	RotateGizmoVertexBuffer = nullptr;
	ScaleGizmoVertexBuffer = nullptr;
}
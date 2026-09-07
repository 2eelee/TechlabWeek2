#include "FGizmo.h"
#include "FVertexSimple.h"
#include "URenderer.h"
#include "GizmoVertices.h"
#include "FMatrix.h"
#include "UCamera.h"

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

	auto TranslateVertices = CreateTranslateGizmoVertices();
	TranslateGizmoVertexCount = static_cast<UINT>(TranslateVertices.size());
	TranslateGizmoVertexBuffer = Renderer.CreateVertexBuffer(
		TranslateVertices.data(),
		static_cast<UINT>(
			TranslateVertices.size() * sizeof(FVertexSimple)
			)
	);

	auto RotateVertices = CreateRotateGizmoVertices();
	RotateGizmoVertexCount = static_cast<UINT>(RotateVertices.size());
	RotateGizmoVertexBuffer = Renderer.CreateVertexBuffer(
		RotateVertices.data(),
		static_cast<UINT>(
			RotateVertices.size() * sizeof(FVertexSimple)
			)
	);

	auto ScaleVertices = CreateScaleGizmoVertices();
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
	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive->GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(Primitive->GetRelativeRotation().y))
		* FMatrix::CreateRotationZ(DegreesToRadians(Primitive->GetRelativeRotation().z));
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
	FMatrix Model = RotationM * TranslationM;

	FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
	Renderer.UpdateConstant(MVP);

	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer.SetAlphaBlend(false);
	Renderer.SetDepthWrite(false);

	Renderer.RenderPrimitive(LocalAxisVertexBuffer, 6);
}

void FGizmo::DrawTransformGizmo(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive)
{
	Renderer.SetAlphaBlend(false);
	Renderer.SetDepthWrite(false);
	Renderer.SetCullMode(D3D11_CULL_NONE);
	Renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (CurrentMode == GizmoMode::Translate)
	{
		FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
		FMatrix Model = TranslationM;

		FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
		Renderer.UpdateConstant(MVP);

		Renderer.RenderPrimitive(TranslateGizmoVertexBuffer, TranslateGizmoVertexCount);
	}

	else if (CurrentMode == GizmoMode::Rotate)
	{
		FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive->GetRelativeRotation().x))
			* FMatrix::CreateRotationY(DegreesToRadians(Primitive->GetRelativeRotation().y))
			* FMatrix::CreateRotationZ(DegreesToRadians(Primitive->GetRelativeRotation().z));
		FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
		FMatrix Model = RotationM * TranslationM;

		FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
		Renderer.UpdateConstant(MVP);

		Renderer.RenderPrimitive(RotateGizmoVertexBuffer, RotateGizmoVertexCount);
	}

	else if (CurrentMode == GizmoMode::Scale)
	{
		FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive->GetRelativeRotation().x))
			* FMatrix::CreateRotationY(DegreesToRadians(Primitive->GetRelativeRotation().y))
			* FMatrix::CreateRotationZ(DegreesToRadians(Primitive->GetRelativeRotation().z));
		FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
		FMatrix Model = RotationM * TranslationM;

		FMatrix MVP = Renderer.CreateMVPFromModel(Model, Camera);
		Renderer.UpdateConstant(MVP);

		Renderer.RenderPrimitive(LocalAxisVertexBuffer, 6);
	}
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
#pragma once

#include "USceneComponent.h"

enum class GizmoMode
{
	Translate,
	Rotate,
	Scale
};

class URenderer;
struct ID3D11Buffer;

class FGizmo
{
public: 
	void CycleMode();
	void Initialize(URenderer& Renderer);
	void DrawGrid(URenderer& Renderer, UCamera* Camera);
	void DrawWorldAxis(URenderer& Renderer, UCamera* Camera);
	void DrawLocalAxis(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive);
	void DrawTransformGizmo(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive);
	void SetGizmoMode(GizmoMode NewMode) { CurrentMode = NewMode; }
	void Release(URenderer& Renderer);

private:
	GizmoMode CurrentMode = GizmoMode::Translate;

	ID3D11Buffer* LocalAxisVertexBuffer = nullptr;
	ID3D11Buffer* WorldAxisVertexBuffer = nullptr;

	ID3D11Buffer* GridVertexBuffer = nullptr;
	UINT GridVertexCount = 0;

	ID3D11Buffer* TranslateGizmoVertexBuffer = nullptr;
	UINT TranslateGizmoVertexCount = 0;

	ID3D11Buffer* RotateGizmoVertexBuffer = nullptr;
	UINT RotateGizmoVertexCount = 0;

	ID3D11Buffer* ScaleGizmoVertexBuffer = nullptr;
	UINT ScaleGizmoVertexCount = 0;
};
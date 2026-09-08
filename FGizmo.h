#pragma once

#include "USceneComponent.h"

enum class GizmoMode
{
	Translate,
	Rotate,
	Scale
};

enum class EGizmoAxis
{
	None,
	X,
	Y,
	Z
};

class URenderer;
struct ID3D11Buffer;
class FMousePicker;

class FGizmo
{
public: 
	void CycleMode();

	void Initialize(URenderer& Renderer);

	void DrawGrid(URenderer& Renderer, UCamera* Camera);
	void DrawWorldAxis(URenderer& Renderer, UCamera* Camera);
	void DrawLocalAxis(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive);
	void DrawTransformGizmo(URenderer& Renderer, UCamera* Camera, UPrimitiveComponent* Primitive, EGizmoAxis HoveredAxis);
	
	void Release(URenderer& Renderer);

	const std::vector<FVertexSimple>& GetCurrentVertices() const;
	FMatrix GetTransformGizmoModel(const UPrimitiveComponent* Primitive) const;

	GizmoMode GetGizmoMode() const { return CurrentMode; }
	void SetGizmoMode(GizmoMode NewMode) { CurrentMode = NewMode; }

private:
	GizmoMode CurrentMode = GizmoMode::Translate;

	std::vector<FVertexSimple> TranslateVertices;
	std::vector<FVertexSimple> RotateVertices;
	std::vector<FVertexSimple> ScaleVertices;

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
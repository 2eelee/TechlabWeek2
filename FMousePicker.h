#pragma once

#include <cfloat>
#include <Windows.h>
#include "FGizmo.h"

struct FIntPoint;
struct FVertexSimple;
class UObject;
class UCamera;
class UPrimitiveComponent;
class FRay;
class FVector3;
struct FMatrix;

class FMousePicker
{
public:
	void HitTestPrimitive(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight);
	void HitTestGizmoAxis(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight, const FGizmo& Gizmo);
	void HandleMouseInput(bool allowWorldInput);
	void ClearHover();

	UPrimitiveComponent* GetClosestPrimitive() const { return ClosestPrimitive; }
	UPrimitiveComponent* GetSelectedPrimitive() const { return SelectedPrimitive; }

	EGizmoAxis GetHoveredAxis() const { return HoveredAxis; }
	EGizmoAxis GetActiveAxis() const { return ActiveAxis; }

private:
	UPrimitiveComponent* ClosestPrimitive = nullptr;
	UPrimitiveComponent* SelectedPrimitive = nullptr;

	FRay TransformRayToLocal(const FRay& worldRay, const FMatrix& modelMatrix);
    bool IntersectTriangleList(const FRay& ray, const FVertexSimple* vertices, UINT vertexCount, float& closestDistance);
    bool IntersectTriangle(const FRay& ray, const FVector3& v0, const FVector3& v1, const FVector3& v2, float& distance);

	EGizmoAxis HoveredAxis = EGizmoAxis::None;
	EGizmoAxis ActiveAxis = EGizmoAxis::None;
};

extern FMousePicker* GMousePicker;
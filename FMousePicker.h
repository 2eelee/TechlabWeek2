#pragma once

#include <cfloat>
#include <Windows.h>

struct FIntPoint;
struct FVertexSimple;
class UObject;
class UCamera;
class UPrimitiveComponent;
class FRay;
class FVector3;

class FMousePicker
{
public:
	void HandleMousePosition(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight);
	UPrimitiveComponent* GetClosestPrimitive() const { return ClosestPrimitive; }
	UPrimitiveComponent* GetSelectedPrimitive() const { return SelectedPrimitive; }

private:
	UPrimitiveComponent* ClosestPrimitive = nullptr;
	UPrimitiveComponent* SelectedPrimitive = nullptr;
    bool IntersectTriangleList(const FRay& ray, const FVertexSimple* vertices, UINT vertexCount, float& closestDistance);
    bool IntersectTriangle(const FRay& ray, const FVector3& v0, const FVector3& v1, const FVector3& v2, float& distance);
};
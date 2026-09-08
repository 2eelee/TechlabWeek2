#pragma once

#include <cfloat>

struct FIntPoint;
class UObject;
class UCamera;
class UPrimitiveComponent;

class FMousePicker
{
public:
	void HandleMousePosition(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight);
	const UPrimitiveComponent* GetClosestPrimitive() const { return ClosestPrimitive; }

private:
	UPrimitiveComponent* ClosestPrimitive = nullptr;
};
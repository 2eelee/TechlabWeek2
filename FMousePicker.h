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
	UPrimitiveComponent* GetClosestPrimitive() const { return ClosestPrimitive; }
	UPrimitiveComponent* GetSelectedPrimitive() const { return SelectedPrimitive; }

private:
	UPrimitiveComponent* ClosestPrimitive = nullptr;
	UPrimitiveComponent* SelectedPrimitive = nullptr;
};

extern FMousePicker* GMousePicker;
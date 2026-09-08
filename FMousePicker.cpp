#include "FMousePicker.h"
#include "UObject.h"
#include "FIntPoint.h"
#include "FRay.h"
#include "UCamera.h"

void FMousePicker::HandleMousePosition(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight)
{
	FRay ray = Camera.ScreenToRay(ScreenPos, (float)ScreenWidth, (float)ScreenHeight);
	UPrimitiveComponent* closestPrimitive = nullptr;
	float hitDistance = 0.0f;
	float closestDistance = FLT_MAX;

	for (UObject* object : GUObjectArray)
	{
		UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
		if (primitive)
		{
			if (primitive->IsHit(ray, hitDistance))
			{
				if (hitDistance < closestDistance)
				{
					closestDistance = hitDistance;
					closestPrimitive = primitive;
				}
			}
		}
	}
	ClosestPrimitive = closestPrimitive;
}
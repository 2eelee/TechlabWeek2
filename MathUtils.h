#pragma once

#include "FIntPoint.h"
#include "FVector2.h"

namespace MathUtils 
{
	inline FVector2 ScreenToNDC(
		const FIntPoint& screenPosition, 
		float viewportWidth, 
		float viewportHeight
	)
	{
		return { 
			(2.0f * screenPosition.X) / viewportWidth - 1.0f, 
			1.0f - (2.0f * screenPosition.Y) / viewportHeight 
		};
	}

	inline float Det3x3(
		float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i
	)
	{
		return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
	}
}
#pragma once

#include "FIntPoint.h"
#include "FVector2.h"

namespace MathUtils 
{
	FVector2 ScreenToNDC(
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
}
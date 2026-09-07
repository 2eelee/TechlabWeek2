#pragma once

#include "Types.h"

struct FIntPoint
{
	int32 X = 0;
	int32 Y = 0;

	FIntPoint operator-(const FIntPoint& Other) const
	{
		return { X - Other.X, Y - Other.Y };
	}
};


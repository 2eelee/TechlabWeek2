#pragma once

#include "Types.h"
#include "Containers.h"
#include "FVector3.h"

struct FMatrix
{
	float m[4][4];

	FMatrix();

	static FMatrix Identity();
	static FMatrix Zero();
	FMatrix operator*(FMatrix other) const;

	static FMatrix CreateScale(float sX, float sY, float sZ);
	static FMatrix CreateRotationX(float angleRad);
	static FMatrix CreateRotationY(float angleRad);
	static FMatrix CreateRotationZ(float angleRad);
	static FMatrix CreateTranslation(float Tx, float Ty, float Tz);
	static FMatrix CreateView(FVector3 Location, FVector3 Right, FVector3 Up, FVector3 Forward);
	static FMatrix CreateProjection(float farZ, float nearZ, float fovrad, float aspectratio);
	static FMatrix CreateOrthogonalProjection(float farZ, float nearZ, float width, float height);
};

inline constexpr float PI = 3.1415926535897932f;
static float DegreesToRadians(float Degrees)
{
	return Degrees * (PI / 180.0f);
}
static float RadiansToDegrees(float Radians)
{
	return Radians * (180.0f / PI);
}




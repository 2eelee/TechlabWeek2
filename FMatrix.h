#pragma once

#include "Types.h"
#include "Containers.h"
#include "MathUtils.h"

struct FVector3;

struct FMatrix
{
	float m[4][4];

	FMatrix()
	{
		for (int i = 0;i < 4;i++)
		{
			for (int j = 0;j < 4;j++) {
				if (i == j) {
					m[i][j] = 1;
				}
				else m[i][j] = 0;
			}
		}
	}

	FMatrix(
		float e00, float e01, float e02, float e03,
		float e10, float e11, float e12, float e13,
		float e20, float e21, float e22, float e23,
		float e30, float e31, float e32, float e33
	)
	{
		m[0][0] = e00; m[0][1] = e01; m[0][2] = e02; m[0][3] = e03;
		m[1][0] = e10; m[1][1] = e11; m[1][2] = e12; m[1][3] = e13;
		m[2][0] = e20; m[2][1] = e21; m[2][2] = e22; m[2][3] = e23;
		m[3][0] = e30; m[3][1] = e31; m[3][2] = e32; m[3][3] = e33;
	}

	FMatrix operator*(FMatrix other) const;

	static FMatrix Zero();
	static FMatrix CreateScale(float sX, float sY, float sZ);
	static FMatrix CreateRotationY(float angleRad);
	static FMatrix CreateRotationX(float angleRad);
	static FMatrix CreateRotationZ(float angleRad);
	static FMatrix CreateTranslation(float Tx, float Ty, float Tz);
	static FMatrix CreateView(FVector3 Location, FVector3 Right, FVector3 Up, FVector3 Forward);
	static FMatrix CreateProjection(float farZ, float nearZ, float fovrad, float aspectratio);
	static FMatrix CreateProjectionInverse(float aspectRatio, float fov, float farZ, float nearZ);
	static FMatrix CreateOrthogonalProjection(float farZ, float nearZ, float width, float height);
	static FMatrix CreateOrthogonalProjectionInverse(float farZ, float nearZ, float width, float height);

	FMatrix Inverse() const;

	inline FMatrix operator*(float scalar) const;
};

static float PI = 3.1415926535897932f;
static float DegreesToRadians(float Degrees)
{
	return Degrees * (PI / 180.0f);
}
static float RadiansToDegrees(float Radians)
{
	return Radians * (180.0f / PI);
}




#pragma once

#include "Types.h"
#include "Containers.h"
#include "FVector3.h"

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

	static FMatrix Zero()
	{
		FMatrix result;
		for (int i = 0;i < 4;i++)
		{
			for (int j = 0;j < 4;j++) {
				result.m[i][j] = 0;
			}
		}
	}

	FMatrix operator*(FMatrix other) const 
	{
		FMatrix output = Zero(); 
		for (int i = 0;i < 4;i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					output.m[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}
		return output;
	}

	static FMatrix CreateScale(float sX, float sY, float sZ)
	{
		FMatrix output = Zero();
		output.m[0][0] = sX;
		output.m[1][1] = sY;
		output.m[2][2] = sZ;
		output.m[3][3] = 1;
		return output;
	}

	static FMatrix CreateRotationX(float angleRad)
	{
		FMatrix output;
		output.m[1][1] = cosf(angleRad);
		output.m[1][2] = sinf(angleRad);
		output.m[2][1] = -sinf(angleRad);
		output.m[2][2] = cosf(angleRad);
		return output;
	}

	static FMatrix CreateRotationY(float angleRad)
	{
		FMatrix output; 
		output.m[0][0] = cosf(angleRad);
		output.m[0][2] = -sinf(angleRad);
		output.m[2][0] = sinf(angleRad);
		output.m[2][2] = cosf(angleRad);
		return output;
	}

	static FMatrix CreateRotationZ(float angleRad)
	{
		FMatrix output;
		output.m[0][0] = cosf(angleRad);
		output.m[0][1] = sinf(angleRad);
		output.m[1][0] = -sinf(angleRad);
		output.m[1][1] = cosf(angleRad);
		return output;
	}
	
	static FMatrix CreateTranslation(float Tx, float Ty, float Tz)
	{
		FMatrix output;
		output.m[3][0] = Tx;
		output.m[3][1] = Ty;
		output.m[3][2] = Tz;
		return output;
	}

	static FMatrix CreateView(float sX, float sY, float sZ)
	{

	}

	static FMatrix CreateProjection(float sX, float sY, float sZ)
	{

	}
};




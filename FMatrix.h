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

	static FMatrix CreateView(FVector3 Location, FVector3 Right, FVector3 Up, FVector3 Forward) 
	{
		FMatrix output;
		output.m[0][0] = Right.x;
		output.m[1][0] = Right.y;
		output.m[2][0] = Right.z;
		output.m[0][1] = Up.x;
		output.m[1][1] = Up.y;
		output.m[2][1] = Up.z;
		output.m[0][2] = Forward.x;
		output.m[1][2] = Forward.y;
		output.m[2][2] = Forward.z;
		output.m[3][0] = -(Location.Dot(Right));
		output.m[3][1] = -(Location.Dot(Up));
		output.m[3][2] = -(Location.Dot(Forward));
		output.m[3][3] = 1;
		return output;
	}

	static FMatrix CreateProjection(float farZ, float nearZ, float fovrad, float aspectratio)  // farZ : 최소 렌더링 시작 거리, nearZ : 최대 렌더링 거리, fovrad: 카메라의 시야각 aspectratio : 종횡비(가로/세로)
	{
		FMatrix output;
		output.m[0][0] = 1 / tanf(fovrad*0.5)/aspectratio;
		output.m[0][1] = 0;
		output.m[0][2] = 0;
		output.m[0][3] = 0;
		output.m[1][0] = 0;
		output.m[1][1] = 1 / tanf(fovrad * 0.5);
		output.m[1][2] = 0;
		output.m[1][3] = 0;
		output.m[2][0] = 0;
		output.m[2][1] = 0;
		output.m[2][2] = farZ / (farZ - nearZ);
		output.m[2][3] = 1;
		output.m[3][0] = 0;
		output.m[3][1] = 0;
		output.m[3][2] = -(nearZ * farZ) / (farZ - nearZ);
		output.m[3][3] = 0;
		return output;
	}
};




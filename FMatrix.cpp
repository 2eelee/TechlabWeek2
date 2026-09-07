#include "FMatrix.h"

FMatrix::FMatrix()
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

FMatrix FMatrix::Identity()
{
	return FMatrix();
}

FMatrix FMatrix::Zero()
{
	FMatrix result;
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++) {
			result.m[i][j] = 0;
		}
	}
	return result;
}

FMatrix FMatrix::operator*(FMatrix other) const
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

FMatrix FMatrix::CreateScale(float sX, float sY, float sZ)
{
	FMatrix output = Zero();
	output.m[0][0] = sX;
	output.m[1][1] = sY;
	output.m[2][2] = sZ;
	output.m[3][3] = 1;
	return output;
}

FMatrix FMatrix::CreateRotationX(float angleRad)
{
	FMatrix output;
	output.m[1][1] = cosf(angleRad);
	output.m[1][2] = sinf(angleRad);
	output.m[2][1] = -sinf(angleRad);
	output.m[2][2] = cosf(angleRad);
	return output;
}

FMatrix FMatrix::CreateRotationY(float angleRad)
{
	FMatrix output;
	output.m[0][0] = cosf(angleRad);
	output.m[0][2] = -sinf(angleRad);
	output.m[2][0] = sinf(angleRad);
	output.m[2][2] = cosf(angleRad);
	return output;
}

FMatrix FMatrix::CreateRotationZ(float angleRad)
{
	FMatrix output;
	output.m[0][0] = cosf(angleRad);
	output.m[0][1] = sinf(angleRad);
	output.m[1][0] = -sinf(angleRad);
	output.m[1][1] = cosf(angleRad);
	return output;
}

FMatrix FMatrix::CreateTranslation(float Tx, float Ty, float Tz)
{
	FMatrix output;
	output.m[3][0] = Tx;
	output.m[3][1] = Ty;
	output.m[3][2] = Tz;
	return output;
}

FMatrix FMatrix::CreateView(FVector3 Location, FVector3 Right, FVector3 Up, FVector3 Forward) // Location : 카메라 위치 Right : 카메라 기준 Right 벡터 Up: 카메라 기준 Up 벡터 Forward: 카메라 기준 앞벡터(Yaw,Pitch 이용하여 유도)
{
	FMatrix output;
	output.m[0][0] = Right.x;
	output.m[1][0] = Right.y;
	output.m[2][0] = Right.z;
	output.m[0][1] = Up.x;
	output.m[1][1] = Up.y;
	output.m[2][1] = Up.z;
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

FMatrix FMatrix::CreateProjection(float farZ, float nearZ, float fovrad, float aspectratio)  // farZ : 최대 렌더링 거리, nearZ : 최소 렌더링 거리, fovrad: 카메라의 가로 시야각 aspectratio : 종횡비(가로/세로)
{
	FMatrix output;
	output.m[0][0] = 1 / tanf(fovrad * 0.5);
	output.m[0][1] = 0;
	output.m[0][2] = 0;
	output.m[0][3] = 0;
	output.m[1][0] = 0;
	output.m[1][1] = 1 / tanf(fovrad * 0.5) * aspectratio;
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
} // XMMatrixPerspectiveFovLH

FMatrix FMatrix::CreateOrthogonalProjection(float farZ, float nearZ, float width, float height)  // farZ : 최소 렌더링 시작 거리, nearZ : 최대 렌더링 거리, fovrad: 카메라의 시야각 aspectratio : 종횡비(가로/세로)
{
	FMatrix output;
	output.m[0][0] = 2 / width;
	output.m[0][1] = 0;
	output.m[0][2] = 0;
	output.m[0][3] = 0;
	output.m[1][0] = 0;
	output.m[1][1] = 2 / height;
	output.m[1][2] = 0;
	output.m[1][3] = 0;
	output.m[2][0] = 0;
	output.m[2][1] = 0;
	output.m[2][2] = 1 / (farZ - nearZ);
	output.m[2][3] = 0;
	output.m[3][0] = 0;
	output.m[3][1] = 0;
	output.m[3][2] = -(nearZ) / (farZ - nearZ);
	output.m[3][3] = 1;
	return output;
}
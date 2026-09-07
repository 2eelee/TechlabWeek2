#include "FMatrix.h"
#include "FVector3.h"

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

FMatrix FMatrix::CreateProjection(float farZ, float nearZ, float fovrad, float aspectratio)  // farZ : 최소 렌더링 시작 거리, nearZ : 최대 렌더링 거리, fovrad: 카메라의 시야각 aspectratio : 종횡비(가로/세로)
{
	FMatrix output;
	output.m[0][0] = 1 / tanf(fovrad * 0.5) / aspectratio;
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
} // XMMatrixPerspectiveFovLH

FMatrix FMatrix::CreateProjectionInverse(float aspectRatio, float fov, float farZ, float nearZ)
{
	float a = aspectRatio * tanf(fov * 0.5);
	float b = tanf(fov * 0.5);
	float c = -(farZ - nearZ) / (nearZ * farZ);
	float d = 1.0f / nearZ;

	return {
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, 0, c,
		0, 0, 1, d
	};
}

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

FMatrix FMatrix::CreateOrthogonalProjectionInverse(float farZ, float nearZ, float width, float height)
{
	float a = 2.0f / width;
	float b = 2.0f / height;
	float c = 1.0f / (farZ - nearZ);
	float d = -nearZ / (farZ - nearZ);

	return {
		1.0f / a, 0, 0, 0,
		0, 1.0f / b, 0, 0,
		0, 0, 1.0f / c, 0,
		0, 0, -d / c, 1
	};
}

inline FMatrix FMatrix::operator*(float scalar) const
{
	return {
		m[0][0] * scalar, m[0][1] * scalar, m[0][2] * scalar, m[0][3] * scalar,
		m[1][0] * scalar, m[1][1] * scalar, m[1][2] * scalar, m[1][3] * scalar,
		m[2][0] * scalar, m[2][1] * scalar, m[2][2] * scalar, m[2][3] * scalar,
		m[3][0] * scalar, m[3][1] * scalar, m[3][2] * scalar, m[3][3] * scalar
	};
}

FMatrix FMatrix::Identity()
{
	return FMatrix();
}

FMatrix FMatrix::Inverse() const
{
	float c00 = MathUtils::Det3x3(
		m[1][1], m[1][2], m[1][3],
		m[2][1], m[2][2], m[2][3],
		m[3][1], m[3][2], m[3][3]
	);

	float c01 = -MathUtils::Det3x3(
		m[1][0], m[1][2], m[1][3],
		m[2][0], m[2][2], m[2][3],
		m[3][0], m[3][2], m[3][3]
	);

	float c02 = MathUtils::Det3x3(
		m[1][0], m[1][1], m[1][3],
		m[2][0], m[2][1], m[2][3],
		m[3][0], m[3][1], m[3][3]
	);

	float c03 = -MathUtils::Det3x3(
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2],
		m[3][0], m[3][1], m[3][2]
	);

	float c10 = -MathUtils::Det3x3(
		m[0][1], m[0][2], m[0][3],
		m[2][1], m[2][2], m[2][3],
		m[3][1], m[3][2], m[3][3]
	);

	float c11 = MathUtils::Det3x3(
		m[0][0], m[0][2], m[0][3],
		m[2][0], m[2][2], m[2][3],
		m[3][0], m[3][2], m[3][3]
	);

	float c12 = -MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][3],
		m[2][0], m[2][1], m[2][3],
		m[3][0], m[3][1], m[3][3]
	);

	float c13 = MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][2],
		m[2][0], m[2][1], m[2][2],
		m[3][0], m[3][1], m[3][2]
	);

	float c20 = MathUtils::Det3x3(
		m[0][1], m[0][2], m[0][3],
		m[1][1], m[1][2], m[1][3],
		m[3][1], m[3][2], m[3][3]
	);

	float c21 = -MathUtils::Det3x3(
		m[0][0], m[0][2], m[0][3],
		m[1][0], m[1][2], m[1][3],
		m[3][0], m[3][2], m[3][3]
	);

	float c22 = MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][3],
		m[1][0], m[1][1], m[1][3],
		m[3][0], m[3][1], m[3][3]
	);

	float c23 = -MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[3][0], m[3][1], m[3][2]
	);

	float c30 = -MathUtils::Det3x3(
		m[0][1], m[0][2], m[0][3],
		m[1][1], m[1][2], m[1][3],
		m[2][1], m[2][2], m[2][3]
	);

	float c31 = MathUtils::Det3x3(
		m[0][0], m[0][2], m[0][3],
		m[1][0], m[1][2], m[1][3],
		m[2][0], m[2][2], m[2][3]
	);

	float c32 = -MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][3],
		m[1][0], m[1][1], m[1][3],
		m[2][0], m[2][1], m[2][3]
	);

	float c33 = MathUtils::Det3x3(
		m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2]
	);

	float determinant = m[0][0] * c00 + m[0][1] * c01 + m[0][2] * c02 + m[0][3] * c03;

	if (abs(determinant) < 0.00001f)
		return FMatrix::Zero();

	FMatrix adjucate = {
		c00, c10, c20, c30,
		c01, c11, c21, c31,
		c02, c12, c22, c32,
		c03, c13, c23, c33
	};

	return adjucate * (1.0f / determinant);
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
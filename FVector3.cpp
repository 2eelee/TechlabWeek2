#include <FMatrix.h>
#include "FVector3.h"

float FVector3::Dot(const FVector3& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

FVector3 FVector3::Cross(const FVector3& other) const
{
	return FVector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

FVector3 FVector3::operator * (const FMatrix& matrix) const
{
	return {
			x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0],
			x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1],
			x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2]
	};
}

FVector3 FVector3::Normalize() const
{
	return FVector3::Normalize(*this);
}

FVector3 FVector3::Normalize(const FVector3& FVector)
{
	float dist = sqrtf(FVector.x * FVector.x + FVector.y * FVector.y + FVector.z * FVector.z);
	if (dist > 0.00001f)
	{
		return FVector3(FVector.x / dist, FVector.y / dist, FVector.z / dist);
	}
	return FVector3(0, 0, 0);
}

FVector3 FVector3::operator+(const FVector3& other) const {
	return FVector3(x + other.x, y + other.y, z + other.z);
}

FVector3 FVector3::operator-(const FVector3& other) const {
	return FVector3(x - other.x, y - other.y, z - other.z);
}

FVector3 FVector3::operator*(float scale) const {
	return FVector3(x * scale, y * scale, z * scale);
}

FVector3 FVector3::operator/(float scale) const {
	if (fabs(scale) > 0.000001f)
	{
		return FVector3(x / scale, y / scale, z / scale);
	}
	return FVector3(0, 0, 0);
}

FVector3& FVector3::operator+=(const FVector3& other) {
	x += other.x; y += other.y; z += other.z;
	return *this;
}

FVector3& FVector3::operator-=(const FVector3& other) {
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

FVector3& FVector3::operator*=(float scale) {
	x *= scale; y *= scale; z *= scale;
	return *this;
}
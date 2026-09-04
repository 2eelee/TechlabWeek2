#pragma once
#include "math.h"

struct FVector3
{
	float x, y, z;
	FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	float Dot(const FVector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	FVector3 Cross(const FVector3& other) const
	{
		return FVector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	static FVector3 Normalize(const FVector3& FVector)
	{
		float dist = sqrtf(FVector.x * FVector.x + FVector.y * FVector.y + FVector.z * FVector.z);
		if (dist > 0.00001f)
		{
			return FVector3(FVector.x / dist, FVector.y / dist, FVector.z / dist);
		}
		return FVector3(0, 0, 0);
	}

	FVector3 operator+(const FVector3& other) const {
		return FVector3(x + other.x, y + other.y, z + other.z);
	}

	FVector3 operator-(const FVector3& other) const {
		return FVector3(x - other.x, y - other.y, z - other.z);
	}

	FVector3 operator*(float scale) const {
		return FVector3(x * scale, y * scale, z * scale);
	}

	FVector3 operator/(float scale) const {
		if (fabs(scale) > 0.000001f) 
		{
			return FVector3(x / scale, y / scale, z / scale);
		}
		return FVector3(0, 0, 0);
	}

	FVector3& operator+=(const FVector3& other) {
		x += other.x; y += other.y; z += other.z;
		return *this;
	}

	FVector3& operator-=(const FVector3& other) {
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}

	FVector3& operator*=(float scale) {
		x *= scale; y *= scale; z *= scale;
		return *this;
	}
};
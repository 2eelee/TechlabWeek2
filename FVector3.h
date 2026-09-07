#pragma once
#include "math.h"

struct FVector3
{
	float x, y, z;
	FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	float Dot(const FVector3& other) const;

	FVector3 Cross(const FVector3& other) const;

	static FVector3 Normalize(const FVector3& FVector);

	FVector3 operator+(const FVector3& other) const;

	FVector3 operator-(const FVector3& other) const;

	FVector3 operator*(float scale) const;

	FVector3 operator/(float scale) const;

	FVector3& operator+=(const FVector3& other);

	FVector3& operator-=(const FVector3& other);

	FVector3& operator*=(float scale);
};
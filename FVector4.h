#pragma once

#include <cmath>

struct FMatrix;
struct FVector4
{
	float X;
	float Y;
	float Z;
	float W;

	float Dot4(const FVector4& other);
	float Dot3(const FVector4& other);
	float LengthSquared();
	float Length3Squared();
	float Length();
	float Length3();
	FVector4 operator - (const FVector4& other) const;
	FVector4 operator * (const FMatrix& matrix) const;
	FVector4 DivideByW() const;
};
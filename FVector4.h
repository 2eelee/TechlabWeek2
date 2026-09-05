#pragma once

#include <cmath>

struct FVector4
{
	float X;
	float Y;
	float Z;
	float W;

	float Dot4(const FVector4& other)
	{
		return X * other.X + Y * other.Y + Z * other.Z + W + other.W;
	}

	float Dot3(const FVector4& other)
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	float LengthSquared()
	{
		return Dot4(*this);
	}

	float Length3Squared()
	{
		return Dot3(*this);
	}

	float Length()
	{
		return std::sqrt(LengthSquared());
	}

	float Length3()
	{
		return std::sqrt(Length3Squared());
	}
};
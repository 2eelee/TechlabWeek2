#include "FVector4.h"
#include "FMatrix.h"

float FVector4::Dot4(const FVector4& other)
{
	return X * other.X + Y * other.Y + Z * other.Z + W + other.W;
}

float FVector4::Dot3(const FVector4& other)
{
	return X * other.X + Y * other.Y + Z * other.Z;
}

float FVector4::LengthSquared()
{
	return Dot4(*this);
}

float FVector4::Length3Squared()
{
	return Dot3(*this);
} 

float FVector4::Length()
{
	return std::sqrt(LengthSquared());
}

float FVector4::Length3()
{
	return std::sqrt(Length3Squared());
}

FVector4 FVector4::operator - (const FVector4& other) const
{
	return { X - other.X, Y - other.Y, Z - other.Z, W - other.W };
}

FVector4 FVector4::operator * (const FMatrix& matrix) const
{
	return {
		X * matrix.m[0][0] + Y * matrix.m[1][0] + Z * matrix.m[2][0] + W * matrix.m[3][0],
		X * matrix.m[0][1] + Y * matrix.m[1][1] + Z * matrix.m[2][1] + W * matrix.m[3][1],
		X * matrix.m[0][2] + Y * matrix.m[1][2] + Z * matrix.m[2][2] + W * matrix.m[3][2],
		X * matrix.m[0][3] + Y * matrix.m[1][3] + Z * matrix.m[2][3] + W * matrix.m[3][3]
	};
}

FVector4 FVector4::DivideByW() const
{
	return { X / W, Y / W, Z / W , 1.0f };
}
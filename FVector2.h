#pragma once

struct FVector2
{
	float X;
	float Y;

	float Dot(const FVector2& other) const { return X * other.X + Y * other.Y; }

	float Length() const { return sqrtf(X * X + Y * Y); }

	FVector2 operator-(const FVector2& other) const {
		return FVector2(X - other.X, Y - other.Y);
	}

	FVector2 Normalize() const
	{
		float length = Length();
		return length > 0.000001f ? FVector2{ X / length, Y / length } : FVector2{ 0.0f, 0.0f };
	}

};
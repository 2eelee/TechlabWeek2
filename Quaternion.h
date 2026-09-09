#pragma once

struct FVector3;
struct FMatrix;
struct Quaternion
{
	float w;
	float x;
	float y;
	float z;

	Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
	Quaternion(const FVector3& euler);
	FVector3 ToEuler() const;
	Quaternion operator* (const Quaternion& other) const;
	inline float Magnitude() const;
	void Normalize();
	Quaternion Conjugate() const;
	FMatrix ToMatrix() const;
};
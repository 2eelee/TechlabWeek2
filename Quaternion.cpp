#include "Quaternion.h"
#include "FMatrix.h"
#include <cmath>
#include <algorithm>
#include "FVector3.h"

Quaternion::Quaternion(const FVector3& euler)
{
	const float halfPitch = euler.x * 0.5;
	const float halfYaw = euler.y * 0.5;
	const float halfRoll = euler.z * 0.5;
	const float cx = std::cos(halfPitch);
	const float sx = std::sin(halfPitch);
	const float cy = std::cos(halfYaw);
	const float sy = std::sin(halfYaw);
	const float cz = std::cos(halfRoll);
	const float sz = std::sin(halfRoll);

	w = cy * cx * cz + sy * sx * sz;
	x = cy * sx * cz + sy * cx * sz;
	y = sy * cx * cz - cy * sx * sz;
	z = cy * cx * sz - sy * sx * cz;
}

Quaternion Quaternion::operator* (const Quaternion& other) const
{
	Quaternion result;
	result.w = (w * other.w) - (x * other.x) - (y * other.y) - (z * other.z);
	result.x = (w * other.x) + (x * other.w) + (y * other.z) - (z * other.y);
	result.y = (w * other.y) - (x * other.z) + (y * other.w) + (z * other.x);
	result.z = (w * other.z) + (x * other.y) - (y * other.x) + (z * other.w);
	return result;
}

float Quaternion::Magnitude() const 
{ 
	return std::sqrt(w * w + x * x + y * y + z * z); 
}

void Quaternion::Normalize()
{
	float magnitude = Magnitude();

	if (magnitude > 0.0f)
	{
		w /= magnitude;
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}
}

Quaternion Quaternion::Conjugate() const
{
	Quaternion result;
	result.w = w;
	result.x = -x;
	result.y = -y;
	result.z = -z;
	return result;
}

FVector3 Quaternion::ToEuler() const
{
	FVector3 euler;
	const float sinPitch = 2.0f * (w * x - y * z);
	constexpr float GIMBAL_THRESHOLD = 0.99999f;
	if (sinPitch >= GIMBAL_THRESHOLD)
	{
		euler.x = 1.57079632679f;
		euler.y = std::atan2(2.0f * (w * y + x * z), 1.0f - 2.0f * (y * y + z * z));
		euler.z = 0.0f;
	}
	else if (sinPitch <= -GIMBAL_THRESHOLD)
	{
		euler.x = -1.57079632679f;
		euler.y = std::atan2(2.0f * (w * y + x * z), 1.0f - 2.0f * (y * y + z * z));
		euler.z = 0.0f;
	}
	else
	{
		euler.x = std::asin(std::clamp(sinPitch, -1.0f, 1.0f));
		euler.y = std::atan2(2.0f * (w * y + x * z), 1.0f - 2.0f * (x * x + y * y));
		euler.z = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (x * x + z * z));
	}
	return euler;
}

FMatrix Quaternion::ToMatrix() const
{
	FMatrix m = FMatrix::Zero();
	Quaternion q = *this;
	q.Normalize();

	const float xx = q.x * q.x;
	const float yy = q.y * q.y;
	const float zz = q.z * q.z;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float yz = q.y * q.z;
	const float wx = q.w * q.x;
	const float wy = q.w * q.y;
	const float wz = q.w * q.z;

	m.m[0][0] = 1.0f - 2.0f * (yy + zz);
	m.m[0][1] = 2.0f * (xy + wz);
	m.m[0][2] = 2.0f * (xz - wy);
	m.m[1][0] = 2.0f * (xy - wz);
	m.m[1][1] = 1.0f - 2.0f * (xx + zz);
	m.m[1][2] = 2.0f * (yz + wx);
	m.m[2][0] = 2.0f * (xz + wy);
	m.m[2][1] = 2.0f * (yz - wx);
	m.m[2][2] = 1.0f - 2.0f * (xx + yy);
	m.m[3][3] = 1.0f;

	return m;
}
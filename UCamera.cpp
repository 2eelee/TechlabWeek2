#pragma once

#include "UCamera.h"
#include "FVector3.h"
#include <algorithm>
#include "FMatrix.h"

FVector3 UCamera::GetForwardVector()
{
	float pitchrad = DegreesToRadians(RelativeRotation.x);
	float yawrad = DegreesToRadians(RelativeRotation.y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[2][0], R.m[2][1], R.m[2][2]);
}

FVector3 UCamera::GetRightVector()
{
	float pitchrad = DegreesToRadians(RelativeRotation.x);
	float yawrad = DegreesToRadians(RelativeRotation.y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[0][0], R.m[0][1], R.m[0][2]);
}

FVector3 UCamera::GetUPVector()
{
	float pitchrad = DegreesToRadians(RelativeRotation.x);
	float yawrad = DegreesToRadians(RelativeRotation.y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[1][0], R.m[1][1], R.m[1][2]);
}

void UCamera::AddPitch(float deltaAngle)
{
	RelativeRotation.x = std::clamp(RelativeRotation.x + deltaAngle, -89.9f, 89.9f);
}

void UCamera::AddYaw(float deltaAngle)
{
	RelativeRotation.y += deltaAngle;
}

void UCamera::AddFov(float deltaAngle)
{
	FovAngle = std::clamp(FovAngle + deltaAngle, 5.0f, 170.0f);
}
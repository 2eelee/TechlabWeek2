#pragma once

#include "UCamera.h"
#include <algorithm>
#include "FMatrix.h"

FVector3 UCamera::GetForwardVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[2][0], R.m[2][1], R.m[2][2]);
}

FVector3 UCamera::GetRightVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[0][0], R.m[0][1], R.m[0][2]);
}

FVector3 UCamera::GetUPVector()
{
	float pitchrad = DegreesToRadians(this->GetRelativeRotation().x);
	float yawrad = DegreesToRadians(this->GetRelativeRotation().y);
	FMatrix R = FMatrix::CreateRotationX(pitchrad) * FMatrix::CreateRotationY(yawrad);
	return FVector3(R.m[1][0], R.m[1][1], R.m[1][2]);
}

void UCamera::AddPitch(float deltaAngle)
{
	FVector3 rot = GetRelativeRotation();
	rot.x = std::clamp(rot.x + deltaAngle, -89.9f, 89.9f);
	SetRelativeRotation(rot);
}

void UCamera::AddYaw(float deltaAngle)
{
	FVector3 rot = GetRelativeRotation();
	rot.y += deltaAngle;
	SetRelativeRotation(rot);
}

void UCamera::AddFov(float deltaAngle)
{
	FovAngle = std::clamp(FovAngle + deltaAngle, 5.0f, 170.0f);
}
#pragma once
#include "USceneComponent.h"

struct UCamera: public USceneComponent
{
	float FarZ;
	float NearZ;
	float FovAngle;
	float AspectRatio;

	UCamera()
	{
		FarZ = 1000.0f;
		NearZ = 1.0f;
		FovAngle = 90.0f;
	}

	virtual FVector3 GetForwardVector();
	virtual FVector3 GetRightVector();
	virtual FVector3 GetUPVector();

	virtual void AddPitch(float pitchradian);
	virtual void AddYaw(float yawradian);
	virtual void AddFov(float fovRad);
};




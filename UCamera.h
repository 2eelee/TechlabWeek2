#pragma once
#include "FVector3.h"
#include "USceneComponent.h"

struct UCamera: public USceneComponent
{
	float FarZ;
	float nearZ;
	float fovangle;
	float aspectratio;

	UCamera()
	{
		FarZ = 1000.0f;
		nearZ = 1.0f;
		fovangle = 90.0f;
	}

	virtual FVector3 GetForwardVector();
	virtual FVector3 GetRightVector();
	virtual FVector3 GetUPVector();

	virtual void Addpitch(float pitchradian);
	virtual void Addyaw(float yawradian);
	virtual void AddFov(float fovRad);
};




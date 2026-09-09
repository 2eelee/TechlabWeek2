#pragma once

#include "FRay.h"
#include "FVector4.h"
#include "USceneComponent.h"

struct UCamera: public USceneComponent
{
	TYPE_DECLARATIONS(UCamera, USceneComponent);
	
	float FarZ;
	float NearZ;
	float FovAngle;
	float orthowidth;
	float CurrentPitch = 0.0f;
	float CurrentYaw = 0.0f;

	bool othogonalEnable = false;

	UCamera()
	{
		FarZ = 1000.0f;
		NearZ = 1.0f;
		FovAngle = 90.0f;
		orthowidth = 15.0f;
	}

	FVector3 GetForwardVector();
	FVector3 GetRightVector();
	FVector3 GetUPVector();
	void SetOrthoWidth(float width);

	void AddPitch(float pitchradian);
    void AddYaw(float yawradian);

	void CamMove(float deltaTime);

	FRay ScreenToRay(FIntPoint ScreenPosition, float ScreenWidth, float ScreenHeight);
	FVector2 WorldToScreen(const FVector3& WorldPosition, float ScreenWidth, float ScreenHeight);
};
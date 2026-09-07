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
	float AspectRatio;

	bool othogonalEnable = false;

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

	FRay ScreenToRay(FIntPoint ScreenPosition, float ScreenWidth, float ScreenHeight)
	{
		FVector2 normalizedPoint = MathUtils::ScreenToNDC(ScreenPosition, ScreenWidth, ScreenHeight);
		FVector4 nearNormalized = { normalizedPoint.X, normalizedPoint.Y, 0.0f, 1.0f };
		FVector4 farNormalized = { normalizedPoint.X, normalizedPoint.Y, 1.0f, 1.0f };

		FMatrix InverseProj;
		if (othogonalEnable)
			InverseProj = FMatrix::CreateOrthogonalProjectionInverse(FarZ, NearZ, 20.0f, 20.0f);
		else InverseProj = FMatrix::CreateProjectionInverse(AspectRatio, DegreesToRadians(FovAngle), FarZ, NearZ);

		FMatrix InverseView = FMatrix::CreateView(GetRelativeLocation(), GetRightVector(), GetUPVector(), GetForwardVector()).Inverse();
		FVector4 deprojectedFar = (farNormalized * InverseProj * InverseView).DivideByW();
		FVector4 deprojectedNear = (nearNormalized * InverseProj * InverseView).DivideByW();
		FVector3 rayOrigin = { deprojectedNear.X, deprojectedNear.Y, deprojectedNear.Z };
		FVector3 rayEnd = { deprojectedFar.X, deprojectedFar.Y, deprojectedFar.Z };
		FVector3 rayDirection = (rayEnd - rayOrigin).Normalize();

		return FRay{ rayOrigin, rayDirection };
	}
};
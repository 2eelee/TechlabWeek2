#pragma once

#include "FMatrix.h"
#include "FVector3.h"
#include "UPrimitiveComponent.h"
#include "UCamera.h"

struct FRenderer
{
	FMatrix CreateModel(UPrimitiveComponent* UPrimitive)
	{
		FMatrix scaleM = FMatrix::CreateScale(UPrimitive->RelativeScale3D.x, UPrimitive->RelativeScale3D.y, UPrimitive->RelativeScale3D.z);
		FMatrix RotationM = FMatrix::CreateRotationX(UPrimitive->RelativeRotation.x) * FMatrix::CreateRotationY(UPrimitive->RelativeRotation.y) * FMatrix::CreateRotationZ(UPrimitive->RelativeRotation.z);
		FMatrix TranslationM = FMatrix::CreateTranslation(UPrimitive->RelativeLocation.x, UPrimitive->RelativeLocation.y, UPrimitive->RelativeLocation.z);
		return scaleM * RotationM * TranslationM;
	}

	FMatrix CreateView(UCamera* cam, FMatrix model)
	{
		return FMatrix::CreateView(cam->RelativeLocation, cam->GetRightVector(), cam->GetUPVector(), cam->GetForwardVector());
	}

	FMatrix CreateProjection(UCamera* cam, FMatrix View)
	{
		return FMatrix::CreateProjection(cam->FarZ, cam->nearZ, DegreesToRadians(cam->fovangle), cam->aspectratio);
	}
	
	FMatrix CreateMVP(FMatrix Model, FMatrix View, FMatrix Projection)
	{
		return Model * View * Projection;
	}
};

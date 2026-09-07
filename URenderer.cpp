#include "FMatrix.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "URenderer.h"

FMatrix URenderer::CreateMVP(UPrimitiveComponent& Primitive, UCamera* Camera)
{
	FMatrix Model = Primitive.GetModelMatrix();
	FMatrix View = FMatrix::CreateView(Camera->GetRelativeLocation(), Camera->GetRightVector(), Camera->GetUPVector(), Camera->GetForwardVector());
	FMatrix Proj;
	if (Camera->othogonalEnable) 
	{
		Proj = FMatrix::CreateOrthogonalProjection(Camera->FarZ, Camera->NearZ, 20.0f, 20.0f);
	}
	else Proj = FMatrix::CreateProjection(Camera->FarZ, Camera->NearZ, DegreesToRadians(Camera->FovAngle), Camera->AspectRatio);
	
	return Model * View * Proj;
}
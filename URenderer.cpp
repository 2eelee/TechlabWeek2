#include "FMatrix.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "URenderer.h"


FMatrix URenderer::CreateMVP(UPrimitiveComponent* Primitive, UCamera* Camera)
{
	FMatrix scaleM = FMatrix::CreateScale(Primitive->GetRelativeScale3D().x, Primitive->GetRelativeScale3D().y, Primitive->GetRelativeScale3D().z);
	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive->GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(Primitive->GetRelativeRotation().y)) 
		* FMatrix::CreateRotationZ(DegreesToRadians(Primitive->GetRelativeRotation().z));
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
	FMatrix Model = scaleM * RotationM * TranslationM;
	FMatrix View = FMatrix::CreateView(Camera->GetRelativeLocation(), Camera->GetRightVector(), Camera->GetUPVector(), Camera->GetForwardVector());
	FMatrix Proj;
	if (Camera->othogonalEnable) 
	{
		Proj = FMatrix::CreateOrthogonalProjection(Camera->FarZ, Camera->NearZ, 20.0f, 20.0f);
	}
	else Proj = FMatrix::CreateProjection(Camera->FarZ, Camera->NearZ, DegreesToRadians(Camera->FovAngle), Camera->AspectRatio);
	
	return Model * View * Proj;
}

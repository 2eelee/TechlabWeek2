#include "FMatrix.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "URenderer.h"


FMatrix URenderer::CreateMVP(UPrimitiveComponent* Primitive, UCamera* Camera)
{
	FMatrix scaleM = FMatrix::CreateScale(Primitive->GetRelativeScale3D().x, Primitive->GetRelativeScale3D().y, Primitive->GetRelativeScale3D().z);
	FMatrix RotationM = FMatrix::CreateRotationX(Primitive->GetRelativeRotation().x) * FMatrix::CreateRotationY(Primitive->GetRelativeRotation().y) * FMatrix::CreateRotationZ(Primitive->GetRelativeRotation().z);
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive->GetRelativeLocation().x, Primitive->GetRelativeLocation().y, Primitive->GetRelativeLocation().z);
	FMatrix Model = scaleM * RotationM * TranslationM;
	FMatrix View = FMatrix::CreateView(Camera->GetRelativeLocation(), Camera->GetRightVector(), Camera->GetUPVector(), Camera->GetForwardVector());
	FMatrix Proj = FMatrix::CreateProjection(Camera->FarZ, Camera->NearZ, DegreesToRadians(Camera->FovAngle), Camera->AspectRatio);
	return Model * View * Proj;
}

#include "FMatrix.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "URenderer.h"

FMatrix URenderer::CreateMVP(UPrimitiveComponent& Primitive, UCamera* Camera)
{
	FMatrix Model = Primitive.GetModelMatrix();
	FMatrix scaleM = FMatrix::CreateScale(Primitive.GetRelativeScale3D().x, Primitive.GetRelativeScale3D().y, Primitive.GetRelativeScale3D().z);
	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(Primitive.GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(Primitive.GetRelativeRotation().y)) 
		* FMatrix::CreateRotationZ(DegreesToRadians(Primitive.GetRelativeRotation().z));
	FMatrix TranslationM = FMatrix::CreateTranslation(Primitive.GetRelativeLocation().x, Primitive.GetRelativeLocation().y, Primitive.GetRelativeLocation().z);

	return CreateMVPFromModel(Model, Camera);
}

FMatrix URenderer::CreateMVPFromModel(const FMatrix& Model, UCamera* Camera)
{
	FMatrix View = FMatrix::CreateView(Camera->GetRelativeLocation(), Camera->GetRightVector(), Camera->GetUPVector(), Camera->GetForwardVector());
	FMatrix Proj;
	if (Camera->othogonalEnable)
	{
		Proj = FMatrix::CreateOrthogonalProjection(Camera->FarZ, Camera->NearZ, Camera->orthowidth, Camera->orthowidth / AspectRatio);
	}
	else Proj = FMatrix::CreateProjection(Camera->FarZ, Camera->NearZ, DegreesToRadians(Camera->FovAngle), AspectRatio);

	return Model * View * Proj;
}
#include "USceneManager.h"

void UPrimitiveComponent::Render(URenderer& renderer)
{
	renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (VertexBuffer && VertexCount > 0)
	{
		renderer.RenderPrimitive(VertexBuffer, VertexCount);
	}
}

bool UPrimitiveComponent::IsHit(FRay& ray, float& hitDistance) const
{
	FMatrix modelInverse = GetModelMatrix().Inverse();
	FVector4 localOrigin = FVector4{ ray.Origin.x, ray.Origin.y, ray.Origin.z, 1.0f } * modelInverse;
	FVector4 localDirection = FVector4{ ray.Direction.x, ray.Direction.y, ray.Direction.z, 0.0f } * modelInverse;
	FVector3 O = { localOrigin.X, localOrigin.Y, localOrigin.Z };
	FVector3 D = FVector3{ localDirection.X, localDirection.Y, localDirection.Z }.Normalize();

	if (!VertexBuffer || !Vertices)
		return false;

	for (int i = 0; i < VertexCount; i += 3)
	{
		const float EPSILON = 0.000001f;
		FVector3 V0 = { Vertices[i].x, Vertices[i].y, Vertices[i].z };
		FVector3 V1 = { Vertices[i + 1].x, Vertices[i + 1].y, Vertices[i + 1].z };
		FVector3 V2 = { Vertices[i + 2].x, Vertices[i + 2].y, Vertices[i + 2].z };
		FVector3 E1 = V1 - V0;
		FVector3 E2 = V2 - V0;
		float determinant = D.Cross(E2).Dot(E1);

		if (determinant > -EPSILON && determinant < EPSILON)
			continue;

		float invDeterminant = 1.0f / determinant;
		FVector3 P = D.Cross(E2);
		FVector3 T = O - V0;
		float u = T.Dot(P) * invDeterminant;

		if (u < 0.0f || u > 1.0)
			continue;

		FVector3 Q = T.Cross(E1);
		float v = D.Dot(Q) * invDeterminant;

		if (v < 0.0f || u + v > 1.0f)
			continue;

		float t = E2.Dot(Q) * invDeterminant;

		if (t < 0.0f)
			continue;

		hitDistance = t;
		return true;
	}

	return false;
}

FMatrix UPrimitiveComponent::GetModelMatrix() const
{
	FMatrix ScaleM = FMatrix::CreateScale(GetRelativeScale3D().x, GetRelativeScale3D().y, GetRelativeScale3D().z);
	FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(GetRelativeRotation().x))
		* FMatrix::CreateRotationY(DegreesToRadians(GetRelativeRotation().y))
		* FMatrix::CreateRotationZ(DegreesToRadians(GetRelativeRotation().z));
	FMatrix TranslationM = FMatrix::CreateTranslation(GetRelativeLocation().x, GetRelativeLocation().y, GetRelativeLocation().z);
	return ScaleM * RotationM * TranslationM;
}
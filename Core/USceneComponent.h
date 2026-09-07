#pragma once

#include "UObject.h"
#include "MeshManager.h"
#include "FRay.h"
#include "FVector4.h"

class URenderer;
class USceneComponent : public UObject
{
	TYPE_DECLARATIONS(USceneComponent, UObject);

public:
	const FVector3 GetRelativeLocation() const { return RelativeLocation; }
	const FVector3 GetRelativeRotation() const { return RelativeRotation; }
	const FVector3 GetRelativeScale3D() const { return RelativeScale3D; }

	void SetRelativeLocation(const FVector3& New) { RelativeLocation = New; }
	void SetRelativeRotation(const FVector3& New) { RelativeRotation = New; }
	void SetRelativeScale3D(const FVector3& New) { RelativeScale3D = New; }

private:
	FVector3 RelativeLocation = FVector3(0.0f, 0.0f, 0.0f);
	FVector3 RelativeRotation = FVector3(0.0f, 0.0f, 0.0f);
	FVector3 RelativeScale3D = FVector3(1.0f, 1.0f, 1.0f);
};

class UPrimitiveComponent : public USceneComponent
{
	TYPE_DECLARATIONS(UPrimitiveComponent, USceneComponent);

public:
	void Render(URenderer& renderer)
	{
		renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (VertexBuffer && VertexCount > 0)
		{
			renderer.RenderPrimitive(VertexBuffer, VertexCount);
		}
	}

	bool IsHit(FRay& ray) const
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
			FVector3 V1 = { Vertices[i+1].x, Vertices[i+1].y, Vertices[i+1].z };
			FVector3 V2 = { Vertices[i+2].x, Vertices[i+2].y, Vertices[i+2].z };
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

			return true;
		}

		return false;
	}

	FMatrix GetModelMatrix() const
	{
		FMatrix ScaleM = FMatrix::CreateScale(GetRelativeScale3D().x, GetRelativeScale3D().y, GetRelativeScale3D().z);
		FMatrix RotationM = FMatrix::CreateRotationX(DegreesToRadians(GetRelativeRotation().x))
			* FMatrix::CreateRotationY(DegreesToRadians(GetRelativeRotation().y))
			* FMatrix::CreateRotationZ(DegreesToRadians(GetRelativeRotation().z));
		FMatrix TranslationM = FMatrix::CreateTranslation(GetRelativeLocation().x, GetRelativeLocation().y, GetRelativeLocation().z);
		return ScaleM * RotationM * TranslationM;
	}

protected:
	ID3D11Buffer* VertexBuffer = nullptr;
	UINT VertexCount = 0;
	FVertexSimple* Vertices = nullptr;
};

class UCubeComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UCubeComp, UPrimitiveComponent);

public:
	UCubeComp() 
	{
		VertexBuffer = MeshManager::Get().GetCubeVertexBuffer();
		VertexCount = MeshManager::Get().CubeVertexCount;
		Vertices = cube_vertices;
	}
};

class USphereComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(USphereComp, UPrimitiveComponent);

public:
	USphereComp()
	{
		VertexBuffer = MeshManager::Get().GetSphereVertexBuffer();
		VertexCount = MeshManager::Get().SphereVertexCount;
		Vertices = sphere_vertices;
	}
};

class UPlaneComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UPlaneComp, UPrimitiveComponent);

public:
	UPlaneComp()
	{
		VertexBuffer = MeshManager::Get().GetPlaneVertexBuffer();
		VertexCount = MeshManager::Get().PlaneVertexCount;
		Vertices = plane_vertices;
	}
};
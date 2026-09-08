#include "FMousePicker.h"
#include "UObject.h"
#include "FIntPoint.h"
#include "FRay.h"
#include "UCamera.h"
#include "InputManager.h"
#include "FMatrix.h"

void FMousePicker::HandleMousePosition(FIntPoint ScreenPos, UCamera& Camera, float ScreenWidth, float ScreenHeight)
{
	UPrimitiveComponent* closestPrimitive = nullptr;
	float closestDistance = FLT_MAX;
	FRay ray = Camera.ScreenToRay(ScreenPos, (float)ScreenWidth, (float)ScreenHeight);

	for (UObject* object : GUObjectArray)
	{
		UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
		if (!primitive)
			continue;

		const FVertexSimple* vertices = primitive->GetVertices();
		UINT vertexCount = primitive->GetVertexCount();
	
		FRay localRay =	TransformRayToLocal(ray, primitive->GetModelMatrix());

		float primitiveDistance;

		if (IntersectTriangleList(localRay, vertices, vertexCount, primitiveDistance))
		{
			if (primitiveDistance < closestDistance)
			{
				closestDistance = primitiveDistance;
				closestPrimitive = primitive;
			}
		}
	}
	ClosestPrimitive = closestPrimitive;
	if (ClosestPrimitive && InputManager::GetInstance().GetMouseButtonDown(MouseButton::LEFT))
	{
		SelectedPrimitive = ClosestPrimitive;
	}
}

FRay FMousePicker::TransformRayToLocal(const FRay& worldRay, const FMatrix& modelMatrix)
{
	FMatrix modelInverse = modelMatrix.Inverse();
	FVector4 localOrigin = FVector4{ worldRay.Origin.x, worldRay.Origin.y, worldRay.Origin.z, 1.0f } * modelInverse;
	FVector4 localDirection = FVector4{ worldRay.Direction.x, worldRay.Direction.y, worldRay.Direction.z, 0.0f } * modelInverse;

	FRay localRay;
	localRay.Origin = FVector3{localOrigin.X, localOrigin.Y, localOrigin.Z};
	localRay.Direction = FVector3{localDirection.X, localDirection.Y, localDirection.Z};

	return localRay;
}

bool FMousePicker::IntersectTriangleList(const FRay& ray, const FVertexSimple* vertices, UINT vertexCount, float& closestDistance)
{
	bool hit = false;
	closestDistance = FLT_MAX;

	for (int i = 0; i + 2 < vertexCount; i += 3)
	{
		const FVertexSimple& a = vertices[i];
		const FVertexSimple& b = vertices[i + 1];
		const FVertexSimple& c = vertices[i + 2];

		FVector3 v0{ a.x, a.y, a.z };
		FVector3 v1{ b.x, b.y, b.z };
		FVector3 v2{ c.x, c.y, c.z };

		float triangleDistance;
		if (IntersectTriangle(ray, v0, v1, v2, triangleDistance))
		{
			hit = true;

			if (triangleDistance < closestDistance)
			{
				closestDistance = triangleDistance;
			}
		}
	}
	return hit;
}

bool FMousePicker::IntersectTriangle(const FRay& ray, const FVector3& v0, const FVector3& v1, const FVector3& v2, float& distance)
{
	FVector3 O = ray.Origin;
	FVector3 D = ray.Direction;

	const float EPSILON = 0.000001f;
	FVector3 E1 = v1 - v0;
	FVector3 E2 = v2 - v0;
	float determinant = D.Cross(E2).Dot(E1);

	if (determinant > -EPSILON && determinant < EPSILON)
		return false;

	float invDeterminant = 1.0f / determinant;

	FVector3 P = D.Cross(E2);
	FVector3 T = O - v0;

	float u = T.Dot(P) * invDeterminant;
	if (u < 0.0f || u > 1.0) return false;

	FVector3 Q = T.Cross(E1);

	float v = D.Dot(Q) * invDeterminant;
	if (v < 0.0f || u + v > 1.0f) return false;

	float t = E2.Dot(Q) * invDeterminant;
	if (t < 0.0f) return false;

	distance = t;
	return true;
}

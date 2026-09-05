#pragma once

#include "URenderer.h"
#include "UObject.h"
#include "FVector3.h"
#include "MeshManager.h"

class USceneComponent : public UObject
{
	TYPE_DECLARATIONS(USceneComponent, UObject);

public:
	const FVector3 GetRelativeLocation() const { return RelativeLocation; }
	const FVector3 GetRelativeRotation() const { return RelativeRotation; }
	const FVector3 GetRelativeScale3D() const { return RelativeScale3D; }

	void SetRelativeLocation(FVector3& New) { RelativeLocation = New; }
	void SetRelativeRotation(FVector3& New) { RelativeRotation = New; }
	void SetRelativeScale3D(FVector3& New) { RelativeScale3D = New; }

private:
	FVector3 RelativeLocation;
	FVector3 RelativeRotation;
	FVector3 RelativeScale3D;
};

class UPrimitiveComponent : public USceneComponent
{
	TYPE_DECLARATIONS(UPrimitiveComponent, USceneComponent);

	void Render(URenderer renderer)
	{
		if (VertexBuffer && VertexCount > 0)
		{
			renderer.RenderPrimitive(VertexBuffer, VertexCount);
		}
	}

protected:
	ID3D11Buffer* VertexBuffer = nullptr;
	UINT VertexCount = 0;
};

class UCubeComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UCubeComp, UPrimitiveComponent);

public:
	UCubeComp() 
	{
		VertexBuffer = MeshManager::Get().GetCubeVertexBuffer();
		VertexCount = MeshManager::Get().CubeVertexCount;
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
	}
};
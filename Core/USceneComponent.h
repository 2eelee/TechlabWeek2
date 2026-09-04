#pragma once

#include "UObject.h"
#include "FVector3.h"

class USceneComponent : public UObject
{
	TYPE_DECLARATIONS(USceneComponent, UObject);
public:
	FVector3 RelativeLocation;
	FVector3 RelativeRotation;
	FVector3 RelativeScale3D;
};

class UPrimitiveComponent : public USceneComponent
{
	TYPE_DECLARATIONS(UPrimitiveComponent, USceneComponent);
};

class UCubeComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UCubeComp, UPrimitiveComponent);
};

class USphereComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(USphereComp, UPrimitiveComponent);
};

class UPlaneComp : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UPlaneComp, UPrimitiveComponent);
};
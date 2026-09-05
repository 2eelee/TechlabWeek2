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

class UCube : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UCube, UPrimitiveComponent);
};

class USphere : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(USphere, UPrimitiveComponent);
};

class UPlane : public UPrimitiveComponent
{
	TYPE_DECLARATIONS(UPlane, UPrimitiveComponent);
};

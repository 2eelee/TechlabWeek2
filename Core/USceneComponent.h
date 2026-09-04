#pragma once

#include "UObject.h"
#include "FVector3.h"

class USceneComponent : public UObject
{
public:
	FVector3 RelativeLocation;
	FVector3 RelativeRotation;
	FVector3 RelativeScale3D;
};

class UPrimitiveComponent : public USceneComponent
{
};

class UCubeComp : public UPrimitiveComponent
{
};

class USphereComp : public UPrimitiveComponent
{
};
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
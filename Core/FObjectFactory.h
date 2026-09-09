#pragma once

#include "UObject.h"

class FObjectFactory 
{
public:
	static UObject* ConstructObject(const UClass* ClassType)
	{
		if (!ClassType)
			return nullptr;

		UObject* object = ClassType->CreateInstance();

		if (!object)
			return nullptr;

		return object;
	}
};
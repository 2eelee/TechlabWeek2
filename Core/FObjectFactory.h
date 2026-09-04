#pragma once

#include "TSubclassOf.h"
#include "UObject.h"

class FObjectFactory {
public:
	static UObject* ConstructObject(TSubclassOf<UObject> ClassType)
	{
		if (!ClassType.IsValid()) { return nullptr;  }

		UObject* object = ClassType.Get()->CreateInstance();

		if (!object) { return nullptr;  }

		return object;
	}
};
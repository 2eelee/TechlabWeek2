#pragma once

#include "UEngineStatics.h"

class UObject
{
public:
	UObject() : UUID(UEngineStatics::GenUUID()) {
		RegisterUObject();
	};

	virtual ~UObject()
	{	
		GUObjectFreeIndices.push_back(InternalIndex);
		GUObjectArray[InternalIndex] = nullptr;
	}

private:
	uint32 UUID;
	uint32 InternalIndex;

	void RegisterUObject()
	{
		if (GUObjectFreeIndices.empty())
		{
			InternalIndex = static_cast<uint32>(GUObjectArray.size());
			GUObjectArray.push_back(this);
		}
		else {
			InternalIndex = GUObjectFreeIndices.back();
			GUObjectFreeIndices.pop_back();
			GUObjectArray[InternalIndex] = this;
		}
	}
};

TArray<UObject*> GUObjectArray;
TArray<uint32> GUObjectFreeIndices;
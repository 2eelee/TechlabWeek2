#pragma once

#include <random>
#include "Types.h"

class UEngineStatics
{
public:
	static uint32 GenUUID()
	{
		static std::random_device RandomDevice;
		std::mt19937 gen(RandomDevice());
		return gen();
	}
};
#pragma once

#include <random>
#include "Types.h"

class UEngineStatics
{
public:
	static uint32 GenUUID()
	{
		std::mt19937 gen(RandomDevice());
		return gen();
	}

private:
	static std::random_device RandomDevice;
};
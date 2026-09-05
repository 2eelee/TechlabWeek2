#pragma once

#include <type_traits>
#include "UClass.h"
#include "UObject.h"

template<
	typename T,
	typename = std::enable_if_t<std::is_base_of_v<UObject, T>>
>
class TSubclassOf 
{
public:
	TSubclassOf() = default;

	TSubclassOf(const ::UClass* from)
	{
		if (from && from->IsChildOf(T::StaticClass()))
		{
			this->Class = from;
		}
	}

	const ::UClass* Get() const { return Class; }
	bool IsValid() const { return Class != nullptr;  }

private:
	const ::UClass* Class = nullptr;
};
#pragma once

#include <cstddef>

#include "Types.h"

struct FMemory
{
public:
    static void* Malloc(std::size_t Size);
    static void Free(void* Ptr);

    static std::size_t GetCurrentMemoryUsage();
    static uint64 GetAllocationCount();

private:
    static std::size_t CurrentMemoryUsage;
    static uint64 AllocationCount;
};
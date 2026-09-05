#include <cstdlib>
#include <malloc.h>
#include <new>
#include "FMemory.h"

// 현재 메모리 크기(bytes) 추적은 _msize를 사용하므로 Windows의 Microsoft CRT에 의존합니다.

std::size_t FMemory::CurrentMemoryUsage = 0;
uint64 FMemory::AllocationCount = 0;

void* FMemory::Malloc(std::size_t Size)
{
	void* Ptr = std::malloc(Size);
	if (Ptr != nullptr)
	{
		AllocationCount++;
		CurrentMemoryUsage += _msize(Ptr);
	}
	return Ptr;
}

void FMemory::Free(void* Ptr)
{
	if (Ptr != nullptr)
	{
		CurrentMemoryUsage -= _msize(Ptr);
		AllocationCount--;
		std::free(Ptr);
	}
}

std::size_t FMemory::GetCurrentMemoryUsage()
{
    return CurrentMemoryUsage;
}

uint64 FMemory::GetAllocationCount()
{
    return AllocationCount;
}

void* operator new(std::size_t Size)
{
	void* Ptr = FMemory::Malloc(Size);
	if (Ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return Ptr;
}

void operator delete(void* Ptr) noexcept
{
	FMemory::Free(Ptr);
}

void* operator new[](std::size_t Size)
{
	void* Ptr = FMemory::Malloc(Size);
	if (Ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return Ptr;
}

void operator delete[](void* Ptr) noexcept
{
	FMemory::Free(Ptr);
}
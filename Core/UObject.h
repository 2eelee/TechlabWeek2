#pragma once

#include "UClass.h"
#include "UEngineStatics.h"

extern TArray<UObject*> GUObjectArray;
extern TArray<uint32> GUObjectFreeIndices;

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

	virtual const UClass* GetClass() const
	{
		return UObject::StaticClass();
	}

	bool IsA(const UClass* Child) const
	{
		if (!Child) { return false; }

		return GetClass()->IsChildOf(Child);
	}

	static const UClass* StaticClass()
	{
		static UClass Class("UObject", nullptr, nullptr);
		return &Class;
	}

	template<typename T, typename U>
	static T* Cast(U* object)
	{
		if (!object) { return nullptr; }

		if (object->IsA(T::StaticClass()))
		{
			return static_cast<T*>(object);
		}

		return nullptr;
	}

	uint32 UUID;
	uint32 InternalIndex;

private:

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

#define TYPE_DECLARATIONS(Type, ParentType)												\
public:																					\
	static UObject* CreateObject()														\
	{																					\
		return new Type();																\
	}																					\
																						\
	static const ::UClass* StaticClass()												\
	{																					\
		static UClass Class(#Type, ParentType::StaticClass(), &Type::CreateObject);		\
		return &Class;																	\
	}																					\
																						\
	virtual const ::UClass* GetClass() const override									\
	{																					\
		return Type::StaticClass();														\
	}
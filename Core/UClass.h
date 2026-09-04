#pragma once

class UObject;

class UClass {
public:
	using CreateFunction = UObject* (*)();

	UClass(const char* name, const UClass* parent, CreateFunction createFunc) 
		: ClassName(name), ParentClass(parent), CreateFunc(createFunc) {}

	UObject* CreateInstance() const
	{
		if (!CreateFunc)
		{
			return nullptr;
		}
		return CreateFunc();
	}

	bool IsChildOf(const UClass* other) const
	{
		for (const UClass* current = this; current; current = current->ParentClass)
		{
			if (current == other)
				return true;
		}
		return false;
	}

private:
	const char* ClassName = nullptr;
	const UClass* ParentClass = nullptr;
	CreateFunction CreateFunc = nullptr;
};
#pragma once

#include "Types.h"
class USceneManager
{
	static USceneManager& GetInstance()
	{
		static USceneManager Instance;
		return Instance;
	}

	USceneManager(const USceneManager&) = delete;
	USceneManager& operator=(const USceneManager&) = delete;

	bool LoadScene(const FString& path)
	{

	}

	bool SaveScene(const FString& path)
	{

	}

private:
	USceneManager() = default;
	~USceneManager() = default;
};
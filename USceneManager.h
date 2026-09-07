#pragma once

#include <fstream>
#include "Types.h"
#include "json.hpp"
#include "UObject.h"
#include "USceneComponent.h"
#include "FConsoleWindow.h"

using json = nlohmann::ordered_json;

class USceneManager
{
public:	
	static USceneManager& GetInstance()
	{
		static USceneManager Instance;
		return Instance;
	}

	USceneManager(const USceneManager&) = delete;
	USceneManager& operator=(const USceneManager&) = delete;

	bool SaveScene(const FString& Scenename);
	
	bool LoadScene(const FString& Scenename);

	bool ClearScene();

private:
	USceneManager() = default;
	~USceneManager() = default;
};
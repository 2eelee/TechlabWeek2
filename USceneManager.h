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

	bool SaveScene(const FString& Scenename)
	{
		FString Scenefilename = "SaveScene\\" + FString(Scenename) + ".Scene";
		json SavedScene;
		int ObjectNum = 0;
		SavedScene["Version"] = 3;
		SavedScene["NextUUID"] = 3;
		for (UObject* object : GUObjectArray)
		{
			UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
			if (primitive)
			{
				FString ObjNum = std::to_string(ObjectNum);
				SavedScene["Primitives"][ObjNum]["Location"] = {primitive->GetRelativeLocation().x,primitive->GetRelativeLocation().y,primitive->GetRelativeLocation().z};
				SavedScene["Primitives"][ObjNum]["Rotation"] = { primitive->GetRelativeRotation().x,primitive->GetRelativeRotation().y,primitive->GetRelativeRotation().z };
				SavedScene["Primitives"][ObjNum]["Scale"] = {primitive->GetRelativeScale3D().x,primitive->GetRelativeScale3D().y,primitive->GetRelativeScale3D().z};
				if (primitive->GetClass() == UCubeComp::StaticClass()) { SavedScene["Primitives"][ObjNum]["Type"] = "Cube"; }
				else if  (primitive->GetClass() == USphereComp::StaticClass()) { SavedScene["Primitives"][ObjNum]["Type"] = "Sphere"; }
				else { SavedScene["Primitives"][ObjNum]["Type"] = "Plane"; }				
				ObjectNum++;
			}
		}
		std::ofstream saveFile(Scenefilename);
		saveFile << SavedScene.dump(4);
		saveFile.close();
		UE_LOG(Log, Success, "Scene Save Complete, Check Scene in SaveScene Folder.");
		return true;
	}

	
	bool LoadScene(const FString& path)
	{
		std::ifstream saveFile("파일명.json");
		if (!saveFile.is_open()) {
			UE_LOG(Log, Error, "Can't Open JSON File");
			return false;
		}

	}

	

private:
	USceneManager() = default;
	~USceneManager() = default;
};
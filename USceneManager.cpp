#include "USceneManager.h"
#include "FObjectFactory.h"
#include "FEditor.h"

bool USceneManager::SaveScene(const FString& Scenename)
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
			SavedScene["Primitives"][ObjNum]["Location"] = { primitive->GetRelativeLocation().x,primitive->GetRelativeLocation().y,primitive->GetRelativeLocation().z };
			SavedScene["Primitives"][ObjNum]["Rotation"] = { primitive->GetRelativeRotation().x,primitive->GetRelativeRotation().y,primitive->GetRelativeRotation().z };
			SavedScene["Primitives"][ObjNum]["Scale"] = { primitive->GetRelativeScale3D().x,primitive->GetRelativeScale3D().y,primitive->GetRelativeScale3D().z };
			if (primitive->GetClass() == UCubeComp::StaticClass()) { SavedScene["Primitives"][ObjNum]["Type"] = "Cube"; }
			else if (primitive->GetClass() == USphereComp::StaticClass()) { SavedScene["Primitives"][ObjNum]["Type"] = "Sphere"; }
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


bool USceneManager::LoadScene(const FString& Scenename)
{
	FString Scenefilename = "SaveScene\\" + FString(Scenename) + ".Scene";
	std::ifstream saveFile(Scenefilename);
	if (!saveFile.is_open()) {
		UE_LOG(Log, Error, "Can't Open JSON File");
		return false;
	}
	json SavedScene;
	saveFile >> SavedScene;
	for (auto& items : SavedScene["Primitives"])
	{
		FString PrimType = items["Type"];

		const UClass* ClassType = nullptr;
		if (PrimType == "Cube") ClassType = UCubeComp::StaticClass();
		if (PrimType == "Sphere") ClassType = USphereComp::StaticClass();
		if (PrimType == "Plane") ClassType = UPlaneComp::StaticClass();

		UObject* PrimObject = FObjectFactory::ConstructObject(ClassType);
		UPrimitiveComponent* Primitive = PrimObject->Cast<UPrimitiveComponent>(PrimObject);
		Primitive->SetRelativeLocation(FVector3(items["Location"][0], items["Location"][1], items["Location"][2]));
		Primitive->SetRelativeRotation(FVector3(items["Rotation"][0], items["Rotation"][1], items["Rotation"][2]));
		Primitive->SetRelativeScale3D(FVector3(items["Scale"][0], items["Scale"][1], items["Scale"][2]));
	}
	saveFile.close();
	UE_LOG(Log, Success, "Load Scene Complete.");
	return true;
}

bool USceneManager::ClearScene()
{
	for (UObject* object : GUObjectArray)
	{
		UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
		if (primitive)
		{
			delete primitive;	
		}
	}
	UE_LOG(Log, Success, "Open New Scene.");
	return true;
}


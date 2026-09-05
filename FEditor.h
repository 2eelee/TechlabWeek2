#pragma once

#include "UObject.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"
#include "FMemory.h"

const char* items[] = { "Sphere", "Cube", "Plane" };

static int currentItem = 0;
static int spawnCount = 0;

class FEditor
{
public:
	void DrawStatUI()
	{
		ImGui::Begin("Stat");
		ImGui::Text("Memory Usage: %zu bytes", FMemory::GetCurrentMemoryUsage());
		ImGui::Text("Allocation Count: %llu", static_cast<unsigned long long>(FMemory::GetAllocationCount()));
		ImGui::End();
	}

	void DrawPropertyUI()
	{
		for (UObject* object : GUObjectArray)
		{
			UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
			if (primitive)
			{
				FVector3 Loc = primitive->GetRelativeLocation();
				FVector3 Rot = primitive->GetRelativeRotation();
				FVector3 Scale = primitive->GetRelativeScale3D();
				ImGui::Begin("Jungle Property Window");
				if (ImGui::InputFloat3("Translation", &Loc.x))
				{
					primitive->SetRelativeLocation(Loc);
				}
				if (ImGui::InputFloat3("Rotation", &Rot.x))
				{
					primitive->SetRelativeRotation(Rot);
				}
				if (ImGui::InputFloat3("Scale", &Scale.x))
				{
					primitive->SetRelativeScale3D(Scale);
				}
				ImGui::End();
			}
		}
	}

	void DrawConsoleUI()
	{
		ImGui::Begin("Stat");
		ImGui::End();
	}

	void DrawControlUI()
	{
		ImGui::Begin("Jungle Control Panel");
		ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));
		if (ImGui::Button("Spawn"))
		{
			spawnCount++;
			switch (currentItem)
			{
			case 0:
			{
				UObject* SphereObj = FObjectFactory::ConstructObject(USphereComp::StaticClass());
				UPrimitiveComponent* primitive = SphereObj->Cast<USphereComp>(SphereObj);
				primitive->SetRelativeLocation(FVector3(5.0f, 0.0f, 0.0f));
				break;
			}
			case 1:
			{
				UObject* CubeObj = FObjectFactory::ConstructObject(UCubeComp::StaticClass());
				UPrimitiveComponent* primitive = CubeObj->Cast<UCubeComp>(CubeObj);
				primitive->SetRelativeLocation(FVector3(-5.0f, 0.0f, 0.0f));
				break;
			}
			case 2:
			{
				UObject* PlaneObj = FObjectFactory::ConstructObject(UPlaneComp::StaticClass());
				UPrimitiveComponent* primitive = PlaneObj->Cast<UPlaneComp>(PlaneObj);
				primitive->SetRelativeLocation(FVector3(0.0f, 0.0f, 0.0f));
				break;
			}
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-120.0f);
		ImGui::InputInt("Number of spawn", &spawnCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
	}
};
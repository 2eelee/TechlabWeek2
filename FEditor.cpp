#pragma once

#include "FEditor.h"
#include "FObjectFactory.h"
#include "FMousePicker.h"

UCamera* Cam;

const char* items[] = { "Sphere", "Cube", "Plane" };
static char name_buffer[32] = "HelloScene";
static int currentItem = 0;

FMousePicker* GMousePicker;

void FEditor::UpdateWindowSize()
{
	display = ImGui::GetIO().DisplaySize;
	cond = GWindowSizeChanged ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
}

void FEditor::DrawStatUI()
{
	ImGui::SetNextWindowPos(ImVec2(bottomX, bottomY + 20), cond, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Stat");
	ImGui::Text("Memory Usage: %zu bytes", FMemory::GetCurrentMemoryUsage());
	ImGui::Text("Allocation Count: %llu", static_cast<unsigned long long>(FMemory::GetAllocationCount()));
	ImGui::End();
}

void FEditor::DrawPropertyUI()
{
	ImGui::SetNextWindowPos(ImVec2(display.x - 20, 20), cond, ImVec2(1.0f, 0.0f));
	UPrimitiveComponent* primitive = GMousePicker->GetSelectedPrimitive();
	if (primitive) {
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

void FEditor::DrawConsoleUI()
{
	bool showConsole = true;

	if (!showConsole || !GConsoleWindow) return;

	ImGui::SetNextWindowPos(ImVec2(20, display.y - 20), cond, ImVec2(0.0f, 1.0f));

	ImGui::SetNextWindowSize(ImVec2(display.x - 40, display.y * 0.3f), cond);

	if (showConsole)
	{
		GConsoleWindow->Draw("Example: Console", &showConsole);
	}
}



void FEditor::DrawControlUI()
{
	ImGui::SetNextWindowPos(ImVec2(20, 20), cond, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Jungle Control Panel");
	ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));
	if (ImGui::Button("Spawn"))
	{
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
			primitive->SetRelativeLocation(FVector3(0.0f, 5.0f, 0.0f));
			break;
		}
		}
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-120.0f);
	spawnCount = 0;
	for (UObject* object : GUObjectArray)
	{
		UPrimitiveComponent* primitive = object->Cast<UPrimitiveComponent>(object);
		if (primitive)
		{
			spawnCount++;
		}
	}
	ImGui::InputInt("Number of spawn", &spawnCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	ImGui::InputText("SceneName", name_buffer, sizeof(name_buffer));
	if (ImGui::Button("New Scene"))
	{
		USceneManager::GetInstance().ClearScene();
	}
	if (ImGui::Button("Save Scene"))
	{
		USceneManager::GetInstance().SaveScene(name_buffer);
	}
	if (ImGui::Button("Load Scene"))
	{
		USceneManager::GetInstance().LoadScene(name_buffer);
	}
	ImGui::Separator();
	for (UObject* object : GUObjectArray)
	{
		UCamera* Camera = object->Cast<UCamera>(object);
		if (Camera)
		{
			Cam = Camera;
			break;
		}
	}
	ImGui::Checkbox("Orthogonal", &Cam->othogonalEnable);
	if (Cam->othogonalEnable)
	{
		ImGui::SliderFloat("Othogonal Width", &Cam->orthowidth, 1.0f, 50.0f);
	}
	ImGui::DragFloat("FOV", &Cam->FovAngle, 1.0f, 5.0f, 170.0f);
	FVector3 CamLoc = Cam->GetRelativeLocation();
	FVector3 CamRot = Cam->GetRelativeRotation();
	if (ImGui::InputFloat3("Camera Location", &CamLoc.x))
	{
		Cam->SetRelativeLocation(CamLoc);
	}
	if (ImGui::InputFloat3("Camera Rotation", &CamRot.x))
	{
		Cam->SetRelativeRotation(CamRot);
	}
	bottomX = ImGui::GetWindowPos().x;
	bottomY = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
	ImGui::End();
}
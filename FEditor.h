#pragma once

#include "UObject.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"
#include "FMemory.h"
#include "UCamera.h"
#include "FConsoleWindow.h"
#include "Window.h"
#include "USceneManager.h"

class FEditor
{
public:
	ImVec2 display;
	ImGuiCond cond;
	bool othogonalEnable = false;
	float bottomX;
	float bottomY;

	void UpdateWindowSize();
	void DrawStatUI();
	void DrawPropertyUI();
	void DrawConsoleUI();
	void DrawControlUI();
};
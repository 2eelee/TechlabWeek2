#pragma once

#include "ImGui/imgui.h"

class FConsoleWindow
{
public:
    FConsoleWindow();
    ~FConsoleWindow();

    void Draw(const char* title, bool* p_open);
    void AddLog(const char* fmt, ...);

    // Main에서 Console / Host 비율 계산할 때 사용
    ImVec2 WindowSize;

private:
    char InputBuf[256];
    ImVector<char*> Items;
    ImVector<const char*> Commands;
    ImVector<char*> History;
    int HistoryPos;

    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;

    void ClearLog();
    void ExecCommand(const char* commandLine);

    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
    int TextEditCallback(ImGuiInputTextCallbackData* data);
};

extern FConsoleWindow* GConsoleWindow;

#define UE_LOG(LogCategory, LogSpec, LogText, ...) \
    do { \
        if (GConsoleWindow) \
            GConsoleWindow->AddLog("[%s][%s] " LogText, #LogCategory, #LogSpec, ##__VA_ARGS__); \
    } while (0)
#pragma once
#pragma once

#include "ImGui/imgui.h"


class FConsoleWindow
{
public:

    // 생성자
    FConsoleWindow();

    // 소멸자
    ~FConsoleWindow();


    // Console Window 그리기
    void Draw(
        const char* title,
        bool* p_open
    );


    // 현재 Console Window 크기
    // Main에서 Console / Host 비율 계산할 때 사용
    ImVec2 WindowSize;


private:

    // 사용자가 현재 입력 중인 문자열
    char InputBuf[256];


    // Console 화면에 출력할 로그
    ImVector<char*> Items;


    // 사용할 수 있는 명령어
    ImVector<const char*> Commands;


    // 이전에 입력했던 명령어
    ImVector<char*> History;


    // ↑ ↓ History 탐색 위치
    int HistoryPos;


    // 로그 검색 Filter
    ImGuiTextFilter Filter;


    // 자동 스크롤 여부
    bool AutoScroll;


    // 다음 프레임에 맨 아래로 이동할지 여부
    bool ScrollToBottom;


private:

    // 로그 전체 삭제
    void ClearLog();


    // 새로운 로그 추가
    void AddLog(
        const char* fmt,
        ...
    );


    // 명령어 처리
    void ExecCommand(
        const char* commandLine
    );


    // InputText Callback 연결
    static int TextEditCallbackStub(
        ImGuiInputTextCallbackData* data
    );


    // TAB 자동완성 / ↑ ↓ History
    int TextEditCallback(
        ImGuiInputTextCallbackData* data
    );
};
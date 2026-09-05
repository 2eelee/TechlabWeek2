#include "FConsoleWindow.h"

#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cstdio>



// 문자열 관련 보조 함수
// ConsoleWindow.cpp 내부에서만 사용

// 문자열 비교
// 대소문자를 구분하지 않고 비교
static int Stricmp(
    const char* s1,
    const char* s2)
{
    int d;

    while (
        (d =
            toupper((unsigned char)*s2) -
            toupper((unsigned char)*s1)) == 0
        && *s1)
    {
        s1++;
        s2++;
    }

    return d;
}


// 문자열 앞부분 비교
// TAB 자동완성에서 사용
static int Strnicmp(
    const char* s1,
    const char* s2,
    int n)
{
    int d = 0;

    while (
        n > 0 &&
        (d =
            toupper((unsigned char)*s2) -
            toupper((unsigned char)*s1)) == 0
        && *s1)
    {
        s1++;
        s2++;
        n--;
    }

    return d;
}


// 문자열 복사
// 새로운 메모리를 만들고 문자열 복사
static char* Strdup(
    const char* s)
{
    size_t len =
        strlen(s) + 1;

    void* buf =
        ImGui::MemAlloc(len);

    return (char*)memcpy(
        buf,
        s,
        len
    );
}


// 문자열 뒤쪽 공백 제거
static void Strtrimblanks(
    char* s)
{
    char* strEnd =
        s + strlen(s);

    while (
        strEnd > s &&
        strEnd[-1] == ' ')
    {
        strEnd--;
    }

    *strEnd = 0;
}


// FConsoleWindow

// 생성자
// Console Window 객체가 만들어질 때 한 번 실행
FConsoleWindow::FConsoleWindow()
{
    // Input 버퍼 초기화
    memset(
        InputBuf,
        0,
        sizeof(InputBuf)
    );


    // History 위치 초기화
    HistoryPos = -1;


    // 자동 스크롤 설정
    AutoScroll = true;

    ScrollToBottom = false;


    // Console Window 크기 초기화
    WindowSize =
        ImVec2(0, 0);


    // 사용할 명령어 등록
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");


    // Console 처음 실행 시 출력할 로그
    AddLog("Hello World!");

    AddLog(
        "Enter 'HELP' for help."
    );
}
// 소멸자

FConsoleWindow::~FConsoleWindow()
{
    // 출력 로그 메모리 해제
    ClearLog();


    // History 메모리 해제
    for (
        int i = 0;
        i < History.Size;
        i++)
    {
        ImGui::MemFree(
            History[i]
        );
    }


    History.clear();
}

// 로그 전체 삭제

void FConsoleWindow::ClearLog()
{
    for (
        int i = 0;
        i < Items.Size;
        i++)
    {
        ImGui::MemFree(
            Items[i]
        );
    }


    Items.clear();
}


// 새로운 로그 추가

void FConsoleWindow::AddLog(
    const char* fmt,
    ...)
{
    char buf[1024];


    va_list args;

    va_start(
        args,
        fmt
    );


    vsnprintf(
        buf,
        IM_COUNTOF(buf),
        fmt,
        args
    );


    va_end(args);


    Items.push_back(
        Strdup(buf)
    );
}

// Console Window 그리기
// 매 프레임 호출

void FConsoleWindow::Draw(
    const char* title,
    bool* pOpen)
{
    // Console Window 시작

    if (!ImGui::Begin(
        title,
        pOpen))
    {
        // 최소화되어 있어도 현재 크기는 저장
        WindowSize =
            ImGui::GetWindowSize();


        ImGui::End();

        return;
    }


    // 현재 Console Window 크기 저장
    WindowSize =
        ImGui::GetWindowSize();

    // Console 설명

    ImGui::TextWrapped(
        "This example implements a console."
    );


    ImGui::TextWrapped(
        "Enter 'HELP' for help."
    );

    // Debug 버튼

    if (ImGui::SmallButton(
        "Add Debug Text"))
    {
        AddLog(
            "some text"
        );
    }


    ImGui::SameLine();

    if (ImGui::SmallButton(
        "Add Debug Error"))
    {
        AddLog(
            "[error] something went wrong"
        );
    }


    ImGui::SameLine();


    if (ImGui::SmallButton(
        "Clear"))
    {
        ClearLog();
    }


    ImGui::SameLine();


    bool copyToClipboard =
        ImGui::SmallButton(
            "Copy"
        );

    // Options

    ImGui::SameLine();


    if (ImGui::Button(
        "Options"))
    {
        ImGui::OpenPopup(
            "Options"
        );
    }


    if (ImGui::BeginPopup(
        "Options"))
    {
        ImGui::Checkbox(
            "Auto-scroll",
            &AutoScroll
        );


        ImGui::EndPopup();
    }

    // Filter

    ImGui::SameLine();


    Filter.Draw(
        "Filter (\"incl,-excl\") (\"error\")",
        180
    );


    ImGui::Separator();

    // 로그 출력 영역
    if (ImGui::BeginChild(
        "ScrollingRegion",
        ImVec2(
            0,
            -ImGui::GetFrameHeightWithSpacing()
        ),
        ImGuiChildFlags_NavFlattened,
        ImGuiWindowFlags_HorizontalScrollbar))
    {
        // Copy 버튼을 눌렀다면
        // 출력되는 로그를 Clipboard로 복사
        if (copyToClipboard)
        {
            ImGui::LogToClipboard();
        }


        // 저장된 모든 로그 출력
        for (
            const char* item :
            Items)
        {
            // Filter를 통과하지 못하면 출력하지 않음
            if (!Filter.PassFilter(
                item))
            {
                continue;
            }


            // Error 로그
            if (strstr(
                item,
                "[error]"))
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text,
                    ImVec4(
                        1.0f,
                        0.4f,
                        0.4f,
                        1.0f
                    )
                );


                ImGui::TextUnformatted(
                    item
                );


                ImGui::PopStyleColor();
            }

            // 일반 로그
            else
            {
                ImGui::TextUnformatted(
                    item
                );
            }
        }


        // Clipboard 복사 종료
        if (copyToClipboard)
        {
            ImGui::LogFinish();
        }

        // 자동 스크롤

        if (
            ScrollToBottom ||
            (
                AutoScroll &&
                ImGui::GetScrollY()
                >=
                ImGui::GetScrollMaxY()
                ))
        {
            ImGui::SetScrollHereY(
                1.0f
            );
        }


        ScrollToBottom = false;
    }


    ImGui::EndChild();


    // Input

    bool reclaimFocus = false;


    ImGuiInputTextFlags inputFlags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackCompletion |
        ImGuiInputTextFlags_CallbackHistory;


    // 사용자가 Input 창에 글자 입력
    //
    // Enter를 누르면 true 반환
    if (ImGui::InputText(
        "Input",
        InputBuf,
        IM_COUNTOF(InputBuf),
        inputFlags,
        &TextEditCallbackStub,
        (void*)this))
    {
        char* s =
            InputBuf;


        // 뒤쪽 공백 제거
        Strtrimblanks(s);


        // 실제 입력 내용이 있다면
        // 명령어 처리
        if (s[0])
        {
            ExecCommand(s);
        }


        // Input 초기화
        s[0] = 0;


        // Enter 이후에도
        // Input에 다시 Focus
        reclaimFocus = true;
    }


    if (reclaimFocus)
    {
        ImGui::SetKeyboardFocusHere(
            -1
        );
    }

    // Console Window 종료

    ImGui::End();
}

// 명령어 처리

void FConsoleWindow::ExecCommand(
    const char* commandLine)
{
    // 사용자가 입력한 명령어도
    // Console에 출력
    AddLog(
        "# %s",
        commandLine
    );


    // History 위치 초기화
    HistoryPos = -1;

    // 같은 명령어가 History에 있다면
    // 기존 명령어 삭제

    for (
        int i =
        History.Size - 1;

        i >= 0;

        i--)
    {
        if (Stricmp(
            History[i],
            commandLine) == 0)
        {
            ImGui::MemFree(
                History[i]
            );


            History.erase(
                History.begin() + i
            );


            break;
        }
    }


    // 새로운 명령어 History에 저장
    History.push_back(
        Strdup(
            commandLine
        )
    );


    // CLEAR
    if (Stricmp(
        commandLine,
        "CLEAR") == 0)
    {
        ClearLog();
    }

    // HELP

    else if (Stricmp(
        commandLine,
        "HELP") == 0)
    {
        AddLog(
            "Commands:"
        );


        for (
            int i = 0;
            i < Commands.Size;
            i++)
        {
            AddLog(
                "- %s",
                Commands[i]
            );
        }
    }

    // HISTORY

    else if (Stricmp(
        commandLine,
        "HISTORY") == 0)
    {
        for (
            int i = 0;
            i < History.Size;
            i++)
        {
            AddLog(
                "%d: %s",
                i,
                History[i]
            );
        }
    }

   // CLASSIFY

    else if (Stricmp(
        commandLine,
        "CLASSIFY") == 0)
    {
        AddLog(
            "CLASSIFY command received."
        );
    }


    // 없는 명령어

    else
    {
        AddLog(
            "Unknown command: '%s'",
            commandLine
        );
    }


    // 새로운 로그가 생겼으므로
    // 다음 프레임에 아래쪽으로 스크롤
    ScrollToBottom = true;
}

// InputText Callback 연결 함수

int FConsoleWindow::TextEditCallbackStub(
    ImGuiInputTextCallbackData* data)
{
    FConsoleWindow* console =
        (FConsoleWindow*)
        data->UserData;


    return console
        ->TextEditCallback(
            data
        );
}

// TAB 자동완성 / ↑ ↓ History
int FConsoleWindow::TextEditCallback(
    ImGuiInputTextCallbackData* data)
{
    switch (
        data->EventFlag)
    {
        // TAB 자동완성

    case ImGuiInputTextFlags_CallbackCompletion:
    {
        const char* wordEnd =
            data->Buf +
            data->CursorPos;


        const char* wordStart =
            wordEnd;


        // 현재 입력하고 있는 단어의
        // 시작 위치 찾기
        while (
            wordStart >
            data->Buf)
        {
            const char c =
                wordStart[-1];


            if (
                c == ' ' ||
                c == '\t' ||
                c == ',' ||
                c == ';')
            {
                break;
            }


            wordStart--;
        }


        // 자동완성 후보 명령어
        ImVector<const char*>
            candidates;


        // Commands에서
        // 현재 입력 문자열과 일치하는 명령어 탐색
        for (
            int i = 0;
            i < Commands.Size;
            i++)
        {
            if (Strnicmp(
                Commands[i],
                wordStart,
                (int)(
                    wordEnd -
                    wordStart
                    )) == 0)
            {
                candidates.push_back(
                    Commands[i]
                );
            }
        }

        // 후보 없음

        if (candidates.Size == 0)
        {
            AddLog(
                "No match for \"%.*s\"!",
                (int)(
                    wordEnd -
                    wordStart
                    ),
                wordStart
            );
        }

        // 후보가 하나
        // 자동완성

        else if (
            candidates.Size == 1)
        {
            // 기존 입력 삭제
            data->DeleteChars(
                (int)(
                    wordStart -
                    data->Buf
                    ),
                (int)(
                    wordEnd -
                    wordStart
                    )
            );


            // 완성된 명령어 삽입
            data->InsertChars(
                data->CursorPos,
                candidates[0]
            );


            // 명령어 뒤에 공백 삽입
            data->InsertChars(
                data->CursorPos,
                " "
            );
        }

        // 후보가 여러 개

        else
        {
            AddLog(
                "Possible matches:"
            );


            for (
                int i = 0;
                i < candidates.Size;
                i++)
            {
                AddLog(
                    "- %s",
                    candidates[i]
                );
            }
        }


        break;
    }

    // ↑ ↓ History

    case ImGuiInputTextFlags_CallbackHistory:
    {
        const int previousHistoryPos =
            HistoryPos;

        // ↑
        // 이전 명령어

        if (
            data->EventKey ==
            ImGuiKey_UpArrow)
        {
            if (HistoryPos == -1)
            {
                HistoryPos =
                    History.Size - 1;
            }

            else if (
                HistoryPos > 0)
            {
                HistoryPos--;
            }
        }

        // ↓
        // 다음 명령어

        else if (
            data->EventKey ==
            ImGuiKey_DownArrow)
        {
            if (HistoryPos != -1)
            {
                HistoryPos++;


                if (
                    HistoryPos >=
                    History.Size)
                {
                    HistoryPos = -1;
                }
            }
        }

        // History 위치가 변경됐다면
        // Input 내용을 변경

        if (
            previousHistoryPos !=
            HistoryPos)
        {
            const char* historyStr =
                HistoryPos >= 0
                ?
                History[HistoryPos]
                :
                "";


            // 기존 Input 내용 제거
            data->DeleteChars(
                0,
                data->BufTextLen
            );


            // 선택한 History 명령어 삽입
            data->InsertChars(
                0,
                historyStr
            );
        }


        break;
    }

    }


    return 0;
}
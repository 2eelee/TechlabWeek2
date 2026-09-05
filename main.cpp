#include <Windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>

#include <cstring>
#include <cctype>
#include <cstdarg>
#include <cstdio>

#include "FVector3.h"
#include "FVertexSimple.h"
#include "URenderer.h"

#include "Window.h"
#include "ImGuiManager.h"
#include "ImGui/imgui.h"

#include "Sphere.h"


// 문자열 비교 함수
// 대소문자를 구분하지 않고 문자열을 비교

static int Stricmp(const char* s1, const char* s2)
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
//
// 새로운 메모리를 만든 뒤 문자열을 복사
static char* Strdup(const char* s)
{
	size_t len = strlen(s) + 1;

	void* buf = ImGui::MemAlloc(len);

	return (char*)memcpy(
		buf,
		s,
		len
	);
}


// 문자열 뒤쪽 공백 제거

static void Strtrimblanks(char* s)
{
	char* str_end =
		s + strlen(s);

	while (
		str_end > s &&
		str_end[-1] == ' ')
	{
		str_end--;
	}

	*str_end = 0;
}


// Console


struct ExampleAppConsole
{
	
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


	// 현재 Console Window 크기
	//
	// Main에서
	//
	// Console크기 / Host크기
	//
	// 를 계산하기 위해 사용
	ImVec2 WindowSize;



	// 생성자
	// Console이 만들어질 때 한 번 실행

	ExampleAppConsole()
	{
		memset(
			InputBuf,
			0,
			sizeof(InputBuf)
		);

		HistoryPos = -1;

		AutoScroll = true;
		ScrollToBottom = false;

		WindowSize = ImVec2(0, 0);


		// 사용할 명령어 등록
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		Commands.push_back("CLASSIFY");


		// 처음 Console을 켰을 때 출력
		AddLog("Hello World!");
		AddLog("Enter 'HELP' for help.");
	}



	// 소멸자


	~ExampleAppConsole()
	{
		ClearLog();

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

	void ClearLog()
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
	

	void AddLog(
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
	//
	// 매 프레임 호출됨

	void Draw(
		const char* title,
		bool* p_open)
	{
		// Console Window 시작
	

		if (!ImGui::Begin(
			title,
			p_open))
		{
			WindowSize =
				ImGui::GetWindowSize();

			ImGui::End();

			return;
		}


		// 현재 Console 크기 저장
		WindowSize =
			ImGui::GetWindowSize();


		// 설명
	

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


		// 로그가 출력되는 영역
	

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
			// 아래에서 출력되는 로그를 Clipboard에 저장
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



		
		// InputText
		
		// 사용자가 글자를 입력
		
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


			// 실제 입력 내용이 있다면 실행
			if (s[0])
			{
				ExecCommand(s);
			}


			// Input 창 초기화
			s[0] = 0;


			// Enter 이후에도
			// 바로 다음 명령어를 입력할 수 있도록 함
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


	void ExecCommand(
		const char* commandLine)
	{
	
		// 사용자가 입력한 내용도 Console에 출력
		

		AddLog(
			"# %s",
			commandLine
		);


		// History 저장
	

		HistoryPos = -1;


		// 같은 명령어가 이미 History에 있다면
		// 이전 것을 삭제
		for (
			int i = History.Size - 1;
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


		History.push_back(
			Strdup(
				commandLine
			)
		);



		// 로그 클리어


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
		//
		// 나중에 원하는 기능 연결 가능
		else if (Stricmp(
			commandLine,
			"CLASSIFY") == 0)
		{
			AddLog(
				"CLASSIFY command received."
			);
		}


		// 없는 명령어 처리 
		

		else
		{
			AddLog(
				"Unknown command: '%s'",
				commandLine
			);
		}


		// 새 로그가 생겼으므로
		// 맨 아래로 이동
		ScrollToBottom = true;
	}



	// InputText Callback 연결 함수


	static int TextEditCallbackStub(
		ImGuiInputTextCallbackData* data)
	{
		ExampleAppConsole* console =
			(ExampleAppConsole*)
			data->UserData;


		return console
			->TextEditCallback(
				data
			);
	}



	// TAB 자동완성 / ↑ ↓ History


	int TextEditCallback(
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


			// 현재 입력하고 있는 단어 시작점 찾기
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



			ImVector<const char*>
				candidates;


			// Commands 중 현재 입력과
			// 일치하는 명령어 탐색
			for (
				int i = 0;
				i < Commands.Size;
				i++)
			{
				if (Strnicmp(
					Commands[i],
					wordStart,
					(int)
					(wordEnd - wordStart))
					== 0)
				{
					candidates.push_back(
						Commands[i]
					);
				}
			}



			// 아무것도 없음
			if (candidates.Size == 0)
			{
				AddLog(
					"No match for \"%.*s\"!",
					(int)
					(wordEnd - wordStart),
					wordStart
				);
			}



			// 하나만 있음
			else if (
				candidates.Size == 1)
			{
				data->DeleteChars(
					(int)
					(wordStart -
						data->Buf),
					(int)
					(wordEnd -
						wordStart)
				);


				data->InsertChars(
					data->CursorPos,
					candidates[0]
				);


				data->InsertChars(
					data->CursorPos,
					" "
				);
			}



			// 여러 개가 있음
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



			// History 위치가 바뀌었다면
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


				data->DeleteChars(
					0,
					data->BufTextLen
				);


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
};


// WinMain


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// Window 생성
	

	FWindow window;

	HWND hWnd =
		window.Create(
			hInstance
		);



	extern URenderer* GRenderer;


	// Renderer 생성
	

	URenderer renderer;


	renderer.Create(
		hWnd
	);


	// Window.cpp에서 사용하는 Renderer와 연결
	GRenderer =
		&renderer;



	// Shader 생성
	renderer.CreateShader();


	// Constant Buffer 생성
	renderer.CreateConstantBuffer();



	// ImGui 생성
	

	FImGuiManager imguiManager;


	imguiManager.Create(
		hWnd,
		renderer.Device,
		renderer.DeviceContext
	);



	// Sphere Vertex Buffer

	UINT numVerticesSphere =
		sizeof(sphere_vertices) /
		sizeof(FVertexSimple);


	ID3D11Buffer*
		vertexBufferSphere =
		renderer.CreateVertexBuffer(
			sphere_vertices,
			sizeof(sphere_vertices)
		);



	// Console 객체
	
	// while 밖에 있어야 로그가 계속 유지됨


	ExampleAppConsole console;


	bool showConsole = true;



	// Console / Host 비율
	//
	// 처음에는
	
	// Width  = Host의 50%
	// Height = Host의 30%


	float consoleWidthRatio =
		0.5f;


	float consoleHeightRatio =
		0.3f;


	bool consoleRatioInitialized =
		false;



	// 프로그램 종료 여부
	

	bool bIsExit = false;



	// FPS

	const int targetFPS = 30;


	const double targetFrameTime =
		1000.0 /
		targetFPS;



	// Timer


	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(
		&frequency
	);


	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;


	double elapsedTime =
		0.0;

	// Main Loop


	while (
		bIsExit == false)
	{
		// Frame 시작 시간


		QueryPerformanceCounter(
			&startTime
		);


		// Windows Message


		MSG msg;


		while (PeekMessage(
			&msg,
			nullptr,
			0,
			0,
			PM_REMOVE))
		{
			TranslateMessage(
				&msg
			);


			DispatchMessage(
				&msg
			);


			if (
				msg.message ==
				WM_QUIT)
			{
				bIsExit = true;

				break;
			}
		}


		if (bIsExit)
		{
			break;
		}


		// Renderer 준비
	

		renderer.Prepare();

		renderer.PrepareShader();


		// ImGui Frame 시작


		imguiManager.BeginFrame();



		ImGuiIO& io =
			ImGui::GetIO();


		// 현재 Host 크기
		//
		// 반드시 while 안에서 매 프레임 다시 가져옴


		float hostWidth =
			io.DisplaySize.x;


		float hostHeight =
			io.DisplaySize.y;

		// Console 첫 크기 설정
		//
		// 처음 한 번:
		
		// Host × 0.5
		// Host × 0.3
	

		if (!consoleRatioInitialized)
		{
			ImGui::SetNextWindowSize(
				ImVec2(
					hostWidth *
					consoleWidthRatio,

					hostHeight *
					consoleHeightRatio
				),
				ImGuiCond_Always
			);
		}



		// Host 크기가 변경됐다면
		//
		// 직전에 저장되어 있던 Console 비율을 이용해서
		// Console 크기도 같이 변경
		

		else if (
			GWindowSizeChanged)
		{
			float newConsoleWidth =
				hostWidth *
				consoleWidthRatio;


			float newConsoleHeight =
				hostHeight *
				consoleHeightRatio;



			ImGui::SetNextWindowSize(
				ImVec2(
					newConsoleWidth,
					newConsoleHeight
				),
				ImGuiCond_Always
			);
		}



	
		// Console Draw
		//
		// Begin()
		// 로그 출력
		// Input
		// Enter
		// End()
		//
		// 전부 여기 안에서 처리
	

		if (showConsole)
		{
			console.Draw(
				"Example: Console",
				&showConsole
			);
		}

		
		// Host 자체를 Resize하고 있는 중이 아니라면
		// 사용자가 Console을 바꾼 결과를 계속 저장
	

		if (
			!GIsResizing &&
			showConsole)
		{
			if (
				hostWidth > 0.0f &&
				hostHeight > 0.0f)
			{
				// 현재 Console Width / Host Width
		

				consoleWidthRatio =
					console.WindowSize.x /
					hostWidth;



				// 현재 Console Height / Host Height
		

				consoleHeightRatio =
					console.WindowSize.y /
					hostHeight;



				consoleRatioInitialized =
					true;
			}
		}

	// 이번 Host Resize 처리는 끝났음
	

		GWindowSizeChanged =
			false;

		// ImGui Frame 종료


		imguiManager.EndFrame();



		
		// Buffer 교환
		

		renderer.SwapBuffer();



		// FPS 제한
		

		do
		{
			Sleep(0);


			QueryPerformanceCounter(
				&endTime
			);


			elapsedTime =
				(
					endTime.QuadPart -
					startTime.QuadPart
					)
				*
				1000.0 /
				frequency.QuadPart;


		} while (
			elapsedTime <
			targetFrameTime);
	}
	// ImGui 소멸
	imguiManager.Release();

	// Vertex Buffer 소멸

	renderer.ReleaseVertexBuffer(
		vertexBufferSphere
	);
	// Constant Buffer 소멸


	renderer.ReleaseConstantBuffer();


	// Shader 소멸


	renderer.ReleaseShader();


	// Renderer 소멸


	renderer.Release();



	return 0;
}
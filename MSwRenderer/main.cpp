#include "MFramework.h"

/*
 MSWRenderer 

 main.cpp
 - WinAPI 프로젝트 구성
 
 MoonAuSosiGi / moonausosigi@gmail.com
*/


// 전역 변수 //////////////////////////////////////////////////////////////////////////////
HWND g_hWnd = NULL;		// 윈도우 핸들
BOOL g_bLoop = TRUE;	// 게임 루프 

char* g_ClassName = "MoonAuSosiGi SW-Renderer";					// 윈도우 클래스 네임
char* g_WindowName = "MoonAuSosiGi SWRenderer Basic Framework"; // 윈도우 네임


//////////////////////////////////////////////////////////////////////////////////////////
BOOL InitWindow(int width, int height);							// 윈도우 초기화 함수 
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);	// 리사이즈시 호출되는 함수 
BOOL MessagePump();												// 메시지 루프
LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);			// 윈도우 프로시저
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
//
// WinMain :: 메인 함수
//
int APIENTRY WinMain(HINSTANCE	hInstance,	// 인스턴스
	HINSTANCE	hPrevInstance,				// 전 프로그램 인스턴스
	LPSTR		lpCmdLine,					// 프로그램 외부에서 받아오는 인자
	int		nCmdShow)						// 응용프로그램 출력 형태 
{

	// ------------------------------------------- //
	// 윈도우 프레임 생성						 
	// ------------------------------------------- //
	if (!InitWindow(800, 600)) 
		return 0;

	// ------------------------------------------- //
	// Framework Setup
	// ------------------------------------------- //
	if (!MFramework::GetInstance()->SetupFramework()) 
		return 0;

	// ------------------------------------------- //
	// SW Renderer 세팅
	// ------------------------------------------- //

	// ------------------------------------------- //
	// 메인 루프
	// ------------------------------------------- //
	while (g_bLoop)
	{
		if (!MessagePump())
			break;

	}
	// ------------------------------------------- //
	// 어플리케이션 종료
	// ------------------------------------------- //
	MFramework::GetInstance()->ReleaseFramework();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// InitWindow : 윈도우 생성 함수
//
// width : 생성할 윈도우 가로 크기 (pixel)
// height : 생성할 윈도우 세로 크기 (pixel)
//
// 입력받은 크기에 맞춰 렌더링 영역을 생성하기 위해
// 클라이언트 영역을 포함 윈도우 전체 크기를 재조정
//
BOOL InitWindow(int width, int height)
{
	// ---------------------------------------------------------------------//
	// 윈도우 클래스 구조체 등록
	// ---------------------------------------------------------------------//
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),CS_CLASSDC,MsgProc,0,0,
		::GetModuleHandle(NULL),NULL,NULL,
		(HBRUSH)GetStockObject(DKGRAY_BRUSH),
		NULL,g_ClassName,NULL
	};

	// 등록
	RegisterClassEx(&wc);

	// ---------------------------------------------------------------------//
	// 윈도우 생성
	// ---------------------------------------------------------------------//
	HWND hWnd = ::CreateWindow(g_ClassName, g_WindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0,
		width, height,
		GetDesktopWindow(), NULL,
		wc.hInstance, NULL
	);
	// 윈도우 생성이 안되었을 경우 
	if (hWnd == NULL) return FALSE;

	// 윈도우 보여주기
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	// 윈도우 핸들 전역변수 복사.
	g_hWnd = hWnd;

	//클라이언트 영역 크기 재조정 
	ResizeWindow(hWnd, width, height);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// void ResizeWindow(HWND hWnd, UINT NewWidth,UINT NewHeight)
//
// 창모드의 경우 해상도 사이즈에 맞추어 전체 윈도우 크기를 재조정
// ex)
// 클라이언트 영역을 들어온 가로 세로값에 맞추어 전체 윈도우의 크기를 재보정
// -> 그래야 게임 이미지가 모두 화면에 보임
// 인자 : 클라이언트 영역의 크기 NewWidth, NewHeight
// 리턴값 없음
//
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight)
{
	// 현재 윈도우의 스타일 구하기
	RECT oldrc;
	DWORD style = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE);

	// 현재 윈도우 '전체' 크기 ( 스크린 좌표 )를 얻는다.
	::GetWindowRect(hWnd, &oldrc);

	//새로 생성될 윈도우의 클라이언트 영역 계산하기
	RECT newrc;
	newrc.left = 0;			newrc.top = 0;
	newrc.right = NewWidth; newrc.bottom = NewHeight;

	//newrc 만큼의 클라이언트 영역을 포함하는 윈도우 전체 크기를 구함
	//메뉴는 없다는 가정하에 처리
	//계산된 결과를 다시 newrc에 넣어준다 ( 스크린 좌표 )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, style, NULL, exstyle);

	//보정된 윈도우의 너비와 폭을 구함
	int width = (newrc.right - newrc.left);
	int height = (newrc.bottom - newrc.top);

	// 새로운 크기를 윈도우에 설정
	::SetWindowPos(hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		width, height, SWP_SHOWWINDOW);

}

//////////////////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump() : 윈도우 메시지 처리 함수.
//
// 보다 빠른 메시지 처리를 위해 메시지 펌프를 수정한다.
// 윈도우즈 OS 로부터 이 어플리케이션으로 특별히 메시지가 날아오지 않는다면
// 게임을 진행.
//
int MessagePump()
{
	MSG msg; ::ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		//메시지 큐에 메시지가 있다면 처리.
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) // 윈도우 종료를 위해 리턴
				return FALSE;
			// 나머지 처리
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else // 특별한 메시지가 없다면 렌더링 진행
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MsgProc 메시지 프로시저
//
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
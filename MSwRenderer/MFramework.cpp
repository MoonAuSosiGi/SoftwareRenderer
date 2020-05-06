#include "MFramework.h"
#include "MSwRenderer.h"
#include "MEngine.h"

MFramework* MFramework::m_instance = nullptr;

MFramework::MFramework()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Single tone GetInstance
//
MFramework* MFramework::GetInstance()
{
	if (m_instance == nullptr)
		m_instance = new MFramework();
	return m_instance;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  플랫폼 세팅 및 데이터 로드. 현재 windows 기준으로 작성되었다.
//
bool MFramework::SetupFramework( int screenWidth, int screenHeight )
{
	// ---------------------------------------------------------------------//
// 윈도우 클래스 구조체 등록
// ---------------------------------------------------------------------//
	WNDCLASSEX wc = {
		sizeof( WNDCLASSEX ),CS_CLASSDC,MsgProc,0,0,
		::GetModuleHandle( NULL ),NULL,NULL,
		( HBRUSH ) GetStockObject( DKGRAY_BRUSH ),
		NULL,m_className,NULL
	};

	// 등록
	RegisterClassEx( &wc );

	// ---------------------------------------------------------------------//
	// 윈도우 생성
	// ---------------------------------------------------------------------//
	HWND hWnd = ::CreateWindow( m_className, m_windowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0,
		screenWidth, screenHeight,
		GetDesktopWindow(), NULL,
		wc.hInstance, NULL
	);
	// 윈도우 생성이 안되었을 경우 
	if ( hWnd == NULL ) return M_FAIL;

	// 윈도우 보여주기
	::ShowWindow( hWnd, SW_SHOWDEFAULT );
	::UpdateWindow( hWnd );

	// 윈도우 핸들 복사.
	m_hWnd = hWnd;

	//클라이언트 영역 크기 재조정 
	ResizeWindow( m_hWnd, screenWidth, screenHeight );

	// SW Renderer 세팅
	if ( MSWRenderer::GetInstance()->SetupDevice( m_hWnd, screenWidth, screenHeight ) == false )
	{
		return M_FAIL;
	}
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// 데이터 해제
//
void MFramework::ReleaseFramework()
{
	MSWRenderer::GetInstance()->ReleaseDevice();
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

void MFramework::ResizeWindow( HWND hWnd, UINT NewWidth, UINT NewHeight )
{
	// 현재 윈도우의 스타일 구하기
	RECT oldrc;
	DWORD style = ( DWORD ) ::GetWindowLong( hWnd, GWL_STYLE );
	DWORD exstyle = ( DWORD ) ::GetWindowLong( hWnd, GWL_EXSTYLE );

	// 현재 윈도우 '전체' 크기 ( 스크린 좌표 )를 얻는다.
	::GetWindowRect( hWnd, &oldrc );

	//새로 생성될 윈도우의 클라이언트 영역 계산하기
	RECT newrc;
	newrc.left = 0;			newrc.top = 0;
	newrc.right = NewWidth; newrc.bottom = NewHeight;

	//newrc 만큼의 클라이언트 영역을 포함하는 윈도우 전체 크기를 구함
	//메뉴는 없다는 가정하에 처리
	//계산된 결과를 다시 newrc에 넣어준다 ( 스크린 좌표 )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx( &newrc, style, NULL, exstyle );

	//보정된 윈도우의 너비와 폭을 구함
	int width = ( newrc.right - newrc.left );
	int height = ( newrc.bottom - newrc.top );

	// 새로운 크기를 윈도우에 설정
	::SetWindowPos( hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		width, height, SWP_SHOWWINDOW );

}

//////////////////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump() : 윈도우 메시지 처리 함수.
//
// 보다 빠른 메시지 처리를 위해 메시지 펌프를 수정한다.
// 윈도우즈 OS 로부터 이 어플리케이션으로 특별히 메시지가 날아오지 않는다면
// 게임을 진행.
//
int MFramework::MessagePump()
{
	MSG msg; ::ZeroMemory( &msg, sizeof( msg ) );

	while ( true )
	{
		//메시지 큐에 메시지가 있다면 처리.
		if ( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT ) // 윈도우 종료를 위해 리턴
				return M_FAIL;
			// 나머지 처리
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		else // 특별한 메시지가 없다면 렌더링 진행
		{
			GameLoop();
			return M_SUCCESS;
		}
	}
	return M_FAIL;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Game Loop
//
void MFramework::GameLoop()
{
	SceneRender();
	SceneUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// 간단한 설명 출력용
//
void MFramework::ShowInfo()
{
	MSWRenderer* rendererInst = MSWRenderer::GetInstance();

	if ( rendererInst == nullptr )
		return;
	// 간단한 오늘의 할 말
	{
		int x = 300, y = 50;
		COLORREF col = RGB( 255, 255, 255 );
		rendererInst->DrawText( x, y, col, "■ %s", m_windowName );

		y += 24;
		char* msg =
			"1.기본프레임웍 구축.\n"
			"2.SW 렌더링 디바이스(Device) 를 생성.\n"
			"3.Idle 시간에 렌더링을 수행.\n"
			"4.Swap(Flipping) chain 을 구현.";
		rendererInst->DrawText( x, y, RGB( 255, 255, 255 ), msg );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// 메시지 프로시저 일단은 프레임워크에서 다루도록 함.
//
LRESULT CALLBACK MFramework::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Scene Render
//
void MFramework::SceneRender()
{
	MSWRenderer* rendererInst = MSWRenderer::GetInstance();

	if ( rendererInst != nullptr )
	{
		rendererInst->BeginScene();
		{
			rendererInst->ClearColor( RGB( 128, 0, 0 ) );
			rendererInst->DrawFPS( 1, 1 );
			ShowInfo();
		}		
		rendererInst->EndScene();
		
		rendererInst->Present();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// Scene Update
//
void MFramework::SceneUpdate()
{

}

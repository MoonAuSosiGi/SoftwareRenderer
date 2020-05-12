#include "MFramework.h"
#include "MASG3MoonAu.h"
#include <stdio.h>

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
int MFramework::SetupFramework( int screenWidth, int screenHeight )
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

	m_displayMode.Width = static_cast< DWORD >( screenWidth );
	m_displayMode.Height = static_cast< DWORD > ( screenHeight );

	// SW Renderer 세팅
	if ( MASG_FAILED( _CreateRenderDevice() ) )
	{
		return M_FAIL;
	}

	// 데이터 로드
	if ( MASG_FAILED( _LoadData() ) )
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
	_ReleaseData();

	// 폰트 제거
	DeleteObject( m_hSysFont );

	// 생성된 역순으로 제거 --> 그렇게 설계되어있다고 가정
	SAFE_DELETE( m_pDevice );
	SAFE_DELETE( m_pMASG );
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
	_SceneRender();
	_SceneUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// 간단한 설명 출력용
//
void MFramework::ShowInfo()
{
	if ( MASG_INVALIED(m_pDevice) )	return;
	// 간단한 오늘의 할 말
	{
		int x = 300, y = 50;
		COLORREF col = RGB( 255, 255, 255 );
		_DrawText( x, y, col, "■ %s", m_windowName );

		y += 24;
		char* msg =
			"1. DirectX 9 Device 흉내 \n"
			"2. Line 그리기";
		_DrawText( x, y, RGB( 255, 255, 255 ), msg );
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
// Create Device -> DirectX 9 디바이스 생성처럼 이루어짐
//
int MFramework::_CreateRenderDevice()
{
	m_pMASG = MASG3MoonAuCreate9( MASG3_VERSION );

	if ( MASG_INVALIED( m_pMASG ) )
	{
		return M_FAIL;
	}

	// 디스플레이 정보 구성
	MASG3PRESENT_PARAMETERS pp;
	ZeroMemory( &pp, sizeof( pp ) );
	pp.Width = m_displayMode.Width;
	pp.Height = m_displayMode.Height;
	pp.BackBuffercnt = 1;				// 백버퍼 갯수 -> 현재 1만 지원
	pp.Windowed = true;					// 창모드 @todo 풀스크린


	// 렌더링 디바이스 생성
	m_pMASG->CreateDevice( m_hWnd,
							&pp,
							MASG3_CREATE_SOFTWARE_VERTEXPROCESSING,
							&m_pDevice
						);

	if ( MASG_INVALIED( m_pDevice ) )
	{
		return M_FAIL;
	}

	// -- 디바이스 기본 상태 옵션 결정할 곳

	// --

	// 시스템 폰트 생성.
	//
	m_hSysFont = CreateFont(
		12, 6,
		0, 0, 1, 0, 0, 0,
		DEFAULT_CHARSET,	//HANGUL_CHARSET  
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		( LPSTR )"굴림"
	);
	if ( MASG_INVALIED( m_hSysFont ) )
	{
		// error..! 
		return M_FAIL;
	}
	SelectObject( m_pDevice->GetRT(), m_hSysFont );

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Scene Render
//
void MFramework::_SceneRender()
{
	if ( MASG_VALIED( m_pDevice ) )
	{
		m_pDevice->BeginScene();
		{
			m_pDevice->ClearColor( RGB( 128, 0, 0 ) );

			// 테스트 드로우!
			_DrawTestObject();

			_DrawFPS(1,1);
			ShowInfo();
		}		
		m_pDevice->EndScene();
		
		m_pDevice->Present();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// Scene Update
//
void MFramework::_SceneUpdate()
{

}

/////////////////////////////////////////////////////////////////////////////////////////
// 
// Draw FPS
//
void MFramework::_DrawFPS( int x, int y )
{
	if ( MASG_INVALIED( m_pDevice ) ) return;

	HDC hdc = m_pDevice->GetRT();

	static int frm = 0;
	static UINT oldtime = GetTickCount();
	static UINT frmcnt = 0;
	static float  fps = 0.0f;

	++frmcnt;

	char msg[80];
	int time = GetTickCount() - oldtime;
	if ( time >= 999 )						// 0~999 밀리세컨드.. 1~1000이 아님
	{
		oldtime = GetTickCount();

		//1초간 증가된 프레임 수를 구합니다..
		frm = frmcnt;	frmcnt = 0;

		//초당 프래임 수를 계산합니다.
		fps = ( float ) ( frm * 1000 ) / ( float ) time;
	}

	SetTextColor( hdc, m_SysFnColor );
	sprintf( msg, "fps=%.1f/%d    ", fps, time );
	TextOut( hdc, x, y, msg, ( int ) strlen( msg ) );
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawText
// TEXT 출력
//
int MFramework::_DrawText( int x, int y, COLORREF col, char* msg, ... )
{
	if ( MASG_INVALIED( m_pDevice ) ) return M_FAIL;

	HDC hdc = m_pDevice->GetRT();
	va_list vl;
	char buff[4096] = "";
	va_start( vl, msg );
	vsprintf( buff, msg, vl );
	RECT rc = { x, y, x + 800, y + 600 };

	SetTextColor( hdc, col );
	int res = ::DrawText( hdc, buff, ( int ) strlen( buff ), &rc, DT_WORDBREAK );
	SetTextColor( hdc, m_SysFnColor );

	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Load Data 실제 필요한 데이터는 여기서 로드
//
int MFramework::_LoadData()
{
	// 정점 버퍼 생성
	_InitVB();
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ReleaseData 데이터 제거
//
void MFramework::_ReleaseData()
{
	SAFE_DELETE( m_pVB );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// InitVB
// 테스트용 객체 버텍스 버퍼 생성
//

struct COLVTX
{
	float x, y;
};
#define FVF_COLVTX (MASG3FVF_XY)

int MFramework::_InitVB()
{
	COLVTX Vertices[]{
		//Face 0 : 정삼각형.★
		{  50.0f, 250.0f },		//v0
		{ 150.0f,  50.0f },		//v1
		{ 250.0f, 250.0f },		//v2
	};

	// 정점 버퍼 생성
	if ( MASG_FAILED( m_pDevice->CreateVertexBuffer(
		sizeof( Vertices ),		// 정점 버퍼 크기 (바이트)
		FVF_COLVTX,				// 정점 규격
		MASG3POOL_SYSTEM,		// 시스템 메모리 사용
		&m_pVB					// 성공시 리턴되는 버퍼 포인터
		) ) )
	{
		return M_FAIL;
	}

	// 버퍼 주소 얻기 -> 실제 DX 와는 역할이 다르지만 동작 결과는 동일하도록 함
	VOID* pBuff;
	if ( MASG_FAILED( m_pVB->Lock( 0, ( void** ) &pBuff ) ) )
	{
		return M_FAIL;
	}

	// 버퍼 정점 데이터 채우기
	memcpy( pBuff, Vertices, sizeof( Vertices ) );

	// DX에서는 획득한 메모리의 관리를 위해 unlock을 하지만 여기선 동작하지 않음. 
	m_pVB->Unlock();

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawTestObject
// SW 렌더러 기능을 하나씩 테스트하는 공간
//
void MFramework::_DrawTestObject()
{
	// 정점 버퍼 등록
	m_pDevice->SetStreamSource( m_pVB, sizeof( COLVTX ) );

	// 정점 규격 설정
	m_pDevice->SetFVF( FVF_COLVTX );

	// 렌더링! 
	m_pDevice->DrawPrimitive( MASG3_TRIANGLELIST, 0, 2 );

}
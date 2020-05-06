#include "MSwRenderer.h"
#include <stdio.h>
#include "mmsystem.h"

MSWRenderer* MSWRenderer::m_instance = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////
//
// Single tone GetInstance
//
MSWRenderer* MSWRenderer::GetInstance()
{
	if ( m_instance == nullptr )
		m_instance = new MSWRenderer();
	return m_instance;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// SetupDevice 
// 렌더타겟 생성 및 기타 렌더러 초기화 로직을 수행
//
int MSWRenderer::SetupDevice( HWND hWnd, int renderTargetWidth, int renderTargetHeight )
{
	CreateRenderTarget( hWnd, renderTargetWidth, renderTargetHeight );

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
	if ( m_hSysFont == NULL )
	{
		// error..! 
		return FALSE;
	}
	SelectObject( m_hSurfaceRT, m_hSysFont );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ReleaseDevice 
// 디바이스 해제
//
void MSWRenderer::ReleaseDevice()
{
	ReleaseRenderTarget();

	// 폰트 제거
	DeleteObject( m_hSysFont );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// _VertexPipeLine
// 버텍스 파이프라인 -> 정점 연산을 수행하고, 결과는 GP 로 넘어간다.
//
int MSWRenderer::_VertexPipeLine()
{
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 
// _GeoMetryPipeLine 
// 지오메트리 파이프라인 -> 기하 연산을 수행하고, 결과는 PP 로 넘어간다.
//
int MSWRenderer::_GeometryPipeLine()
{
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// _PixelPipeLine
// 픽셀 파이프라인 -> 픽셀당 연산을 수행하고, 결과는 RT에 기록된다.
//
int MSWRenderer::_PixelPipeLine()
{
	return M_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// CreateRenderTarget 
// SW 렌더러 렌더 타겟 생성
//
int MSWRenderer::CreateRenderTarget( HWND hWnd, int renderTargetWidth, int renderTargetHeight )
{
	HDC hdc = GetDC( hWnd );
	m_hSurfaceRT = CreateCompatibleDC( hdc );
	m_hBmpRT = ( HBITMAP ) CreateCompatibleBitmap( hdc, renderTargetWidth, renderTargetHeight );
	SelectObject( m_hSurfaceRT, m_hBmpRT );
	ReleaseDC( hWnd, hdc );

	m_hWnd = hWnd;
	m_nRenderTargetWidth = renderTargetWidth;
	m_nRenderTargetHeight = renderTargetHeight;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
//
// ReleaseRenderTarget
// 렌더타겟 제거
//
void MSWRenderer::ReleaseRenderTarget()
{
	DeleteObject( m_hBmpRT );
	DeleteDC( m_hSurfaceRT );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// BeginScene
// 렌더 직전 작업 수행
//
int MSWRenderer::BeginScene()
{
	//폰트 배경색 기본값 
	SetBkMode( m_hSurfaceRT, TRANSPARENT );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// EndScene
// 렌더링 종료 후 작업 수행
//
int MSWRenderer::EndScene()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Present
// RT의 내용을 Front Buffer에 출력
//
int MSWRenderer::Present()
{
	HDC hdc = GetDC( m_hWnd );
	BitBlt( hdc, 0, 0, m_nRenderTargetWidth,m_nRenderTargetHeight, m_hSurfaceRT, 0, 0, SRCCOPY );
	ReleaseDC( m_hWnd, hdc );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ClearColor
// 렌더타겟 클리어
//
int MSWRenderer::ClearColor( COLORREF color )
{
	HBRUSH hBrush = CreateSolidBrush( color );
	RECT rc = { 0, 0, m_nRenderTargetWidth,  m_nRenderTargetHeight };
	FillRect( m_hSurfaceRT, &rc, hBrush );
	DeleteObject( hBrush );

	m_bkColor = color;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawFPS
// FPS 출력
//
void MSWRenderer::DrawFPS( int x, int y )
{
	HDC hdc = m_hSurfaceRT;

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
	TextOut( hdc, x, y, msg, (int)strlen( msg ) );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawText
// TEXT 출력
//
int MSWRenderer::DrawText( int x, int y, COLORREF col, char* msg, ... )
{
	va_list vl;
	char buff[4096] = "";
	va_start( vl, msg );
	vsprintf( buff, msg, vl );
	RECT rc = { x, y, x + 800, y + 600 };

	SetTextColor( m_hSurfaceRT, col );
	int res = ::DrawText( m_hSurfaceRT, buff, (int)strlen( buff ), &rc, DT_WORDBREAK );
	SetTextColor( m_hSurfaceRT, m_SysFnColor );

	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawPrimitive
// 기하 데이터 렌더링
//
// 이 메소드 호출 전에 정점버퍼 / FVF / 마디 크기 (Stride) 등 렌더링에 필요한 정보가
// 세팅되어 있어야 함
//
int MSWRenderer::DrawPrimitive( MASG3PRIMITIVETYPE primitiveType, UINT StartVertex, UINT PrimitiveCount )
{
	//if(MASG_INVALIED() )

	return M_SUCCESS;
}
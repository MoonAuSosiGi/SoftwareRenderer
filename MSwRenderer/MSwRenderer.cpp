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
// ����Ÿ�� ���� �� ��Ÿ ������ �ʱ�ȭ ������ ����
//
int MSWRenderer::SetupDevice( HWND hWnd, int renderTargetWidth, int renderTargetHeight )
{
	CreateRenderTarget( hWnd, renderTargetWidth, renderTargetHeight );

	// �ý��� ��Ʈ ����.
	//
	m_hSysFont = CreateFont(
		12, 6,
		0, 0, 1, 0, 0, 0,
		DEFAULT_CHARSET,	//HANGUL_CHARSET  
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		( LPSTR )"����"
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
// ����̽� ����
//
void MSWRenderer::ReleaseDevice()
{
	ReleaseRenderTarget();

	// ��Ʈ ����
	DeleteObject( m_hSysFont );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// CreateRenderTarget 
// SW ������ ���� Ÿ�� ����
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
// ����Ÿ�� ����
//
void MSWRenderer::ReleaseRenderTarget()
{
	DeleteObject( m_hBmpRT );
	DeleteDC( m_hSurfaceRT );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// BeginScene
// ���� ���� �۾� ����
//
int MSWRenderer::BeginScene()
{
	//��Ʈ ���� �⺻�� 
	SetBkMode( m_hSurfaceRT, TRANSPARENT );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// EndScene
// ������ ���� �� �۾� ����
//
int MSWRenderer::EndScene()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Present
// RT�� ������ Front Buffer�� ���
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
// ����Ÿ�� Ŭ����
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
// FPS ���
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
	if ( time >= 999 )						// 0~999 �и�������.. 1~1000�� �ƴ�
	{
		oldtime = GetTickCount();

		//1�ʰ� ������ ������ ���� ���մϴ�..
		frm = frmcnt;	frmcnt = 0;

		//�ʴ� ������ ���� ����մϴ�.
		fps = ( float ) ( frm * 1000 ) / ( float ) time;
	}

	SetTextColor( hdc, m_SysFnColor );
	sprintf( msg, "fps=%.1f/%d    ", fps, time );
	TextOut( hdc, x, y, msg, (int)strlen( msg ) );
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// DrawText
// TEXT ���
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
#pragma once

#include <windows.h>
class MSWRenderer
{
private:
	static MSWRenderer* m_instance;

	// 렌더타겟 구성 핸들
	HWND m_hWnd;
	HBITMAP m_hBmpRT;
	HDC m_hSurfaceRT;
	COLORREF m_bkColor = RGB( 0, 0, 255 );
	int m_nRenderTargetWidth = 0;
	int m_nRenderTargetHeight = 0;

	// 시스템 폰트
	HFONT m_hSysFont;
	COLORREF m_SysFnColor = RGB( 0, 255, 0 );
private:
	MSWRenderer() {}
	~MSWRenderer() { ReleaseDevice(); }

	int CreateRenderTarget( HWND hWnd, int renderTargetWidth, int renderTargetHeight );
	void ReleaseRenderTarget();
public:
	static MSWRenderer* GetInstance();

	int SetupDevice( HWND hWnd, int renderTargetWidth, int renderTargetHeight );
	void ReleaseDevice();

	int BeginScene();
	int EndScene();
	int Present();
	int ClearColor( COLORREF color );

	void DrawFPS( int x, int y );
	int DrawText( int x, int y, COLORREF col, char* msg, ... );
	
};
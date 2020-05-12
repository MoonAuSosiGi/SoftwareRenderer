#pragma once

/*
	MFramework 
	Singletone class.
	드로잉 / 업데이트를 담당합니다.

*/


#include <Windows.h>
#include "MASG3MoonAu.h"

class MFramework
{
private:
	static  MFramework* m_instance;
	
	// windows ------------------//
	HWND m_hWnd;
	BOOL m_bLoop = true;
	LPSTR m_className = "MoonAuSosiGi SW-Renderer";
	LPSTR m_windowName = "MoonAuSosiGi SW-Renderer";

	MASG3DISPLAYMODE m_displayMode;
	// MoonAu
	LPMASG3MOONAU m_pMASG = nullptr;
	// Device
	LPMASG3DEVICE9 m_pDevice = nullptr;	

	// 시스템 폰트
	HFONT m_hSysFont;
	COLORREF m_SysFnColor = RGB( 0, 255, 0 );

private:
	MFramework();
public:
	static MFramework* GetInstance();
	
	int SetupFramework( int screenWidth, int screenHeight );
	void ReleaseFramework();

	// 리사이즈시 호출되는 함수 
	void ResizeWindow( HWND hWnd, UINT NewWidth, UINT NewHeight );	
	int MessagePump();
	// ----------------------- //
	// Game Loop
	// ----------------------- //
	void GameLoop();

	bool IsGameLoop() { return m_bLoop; }

	// 간단한 설명 출력
	void ShowInfo();

public:
	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:
	int _CreateRenderDevice();
	void _SceneRender();
	void _SceneUpdate();
	void _DrawFPS( int x, int y );
	int _DrawText( int x, int y, COLORREF col, char* msg, ... );

	void _DrawTestObject();
};
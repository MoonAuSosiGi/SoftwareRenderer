#include "MFramework.h"
#include "MSwRenderer.h"

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
//  �÷��� ���� �� ������ �ε�. ���� windows �������� �ۼ��Ǿ���.
//
bool MFramework::SetupFramework( int screenWidth, int screenHeight )
{
	// ---------------------------------------------------------------------//
// ������ Ŭ���� ����ü ���
// ---------------------------------------------------------------------//
	WNDCLASSEX wc = {
		sizeof( WNDCLASSEX ),CS_CLASSDC,MsgProc,0,0,
		::GetModuleHandle( NULL ),NULL,NULL,
		( HBRUSH ) GetStockObject( DKGRAY_BRUSH ),
		NULL,m_className,NULL
	};

	// ���
	RegisterClassEx( &wc );

	// ---------------------------------------------------------------------//
	// ������ ����
	// ---------------------------------------------------------------------//
	HWND hWnd = ::CreateWindow( m_className, m_windowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0,
		screenWidth, screenHeight,
		GetDesktopWindow(), NULL,
		wc.hInstance, NULL
	);
	// ������ ������ �ȵǾ��� ��� 
	if ( hWnd == NULL ) return FALSE;

	// ������ �����ֱ�
	::ShowWindow( hWnd, SW_SHOWDEFAULT );
	::UpdateWindow( hWnd );

	// ������ �ڵ� ����.
	m_hWnd = hWnd;

	//Ŭ���̾�Ʈ ���� ũ�� ������ 
	ResizeWindow( m_hWnd, screenWidth, screenHeight );

	// SW Renderer ����
	if ( MSWRenderer::GetInstance()->SetupDevice( m_hWnd, screenWidth, screenHeight ) == false )
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ������ ����
//
void MFramework::ReleaseFramework()
{
	MSWRenderer::GetInstance()->ReleaseDevice();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// void ResizeWindow(HWND hWnd, UINT NewWidth,UINT NewHeight)
//
// â����� ��� �ػ� ����� ���߾� ��ü ������ ũ�⸦ ������
// ex)
// Ŭ���̾�Ʈ ������ ���� ���� ���ΰ��� ���߾� ��ü �������� ũ�⸦ �纸��
// -> �׷��� ���� �̹����� ��� ȭ�鿡 ����
// ���� : Ŭ���̾�Ʈ ������ ũ�� NewWidth, NewHeight
// ���ϰ� ����
//

void MFramework::ResizeWindow( HWND hWnd, UINT NewWidth, UINT NewHeight )
{
	// ���� �������� ��Ÿ�� ���ϱ�
	RECT oldrc;
	DWORD style = ( DWORD ) ::GetWindowLong( hWnd, GWL_STYLE );
	DWORD exstyle = ( DWORD ) ::GetWindowLong( hWnd, GWL_EXSTYLE );

	// ���� ������ '��ü' ũ�� ( ��ũ�� ��ǥ )�� ��´�.
	::GetWindowRect( hWnd, &oldrc );

	//���� ������ �������� Ŭ���̾�Ʈ ���� ����ϱ�
	RECT newrc;
	newrc.left = 0;			newrc.top = 0;
	newrc.right = NewWidth; newrc.bottom = NewHeight;

	//newrc ��ŭ�� Ŭ���̾�Ʈ ������ �����ϴ� ������ ��ü ũ�⸦ ����
	//�޴��� ���ٴ� �����Ͽ� ó��
	//���� ����� �ٽ� newrc�� �־��ش� ( ��ũ�� ��ǥ )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx( &newrc, style, NULL, exstyle );

	//������ �������� �ʺ�� ���� ����
	int width = ( newrc.right - newrc.left );
	int height = ( newrc.bottom - newrc.top );

	// ���ο� ũ�⸦ �����쿡 ����
	::SetWindowPos( hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		width, height, SWP_SHOWWINDOW );

}

//////////////////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump() : ������ �޽��� ó�� �Լ�.
//
// ���� ���� �޽��� ó���� ���� �޽��� ������ �����Ѵ�.
// �������� OS �κ��� �� ���ø����̼����� Ư���� �޽����� ���ƿ��� �ʴ´ٸ�
// ������ ����.
//
int MFramework::MessagePump()
{
	MSG msg; ::ZeroMemory( &msg, sizeof( msg ) );

	while ( true )
	{
		//�޽��� ť�� �޽����� �ִٸ� ó��.
		if ( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT ) // ������ ���Ḧ ���� ����
				return FALSE;
			// ������ ó��
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		else // Ư���� �޽����� ���ٸ� ������ ����
		{
			GameLoop();
			return TRUE;
		}
	}
	return FALSE;
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
// ������ ���� ��¿�
//
void MFramework::ShowInfo()
{
	MSWRenderer* rendererInst = MSWRenderer::GetInstance();

	if ( rendererInst == nullptr )
		return;
	// ������ ������ �� ��
	{
		int x = 300, y = 50;
		COLORREF col = RGB( 255, 255, 255 );
		rendererInst->DrawText( x, y, col, "�� %s", m_windowName );

		y += 24;
		char* msg =
			"1.�⺻�����ӿ� ����.\n"
			"2.SW ������ ����̽�(Device) �� ����.\n"
			"3.Idle �ð��� �������� ����.\n"
			"4.Swap(Flipping) chain �� ����.";
		rendererInst->DrawText( x, y, RGB( 255, 255, 255 ), msg );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// �޽��� ���ν��� �ϴ��� �����ӿ�ũ���� �ٷ絵�� ��.
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

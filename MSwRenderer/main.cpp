#include "MFramework.h"

/*
 MSWRenderer 

 main.cpp
 - WinAPI ������Ʈ ����
 
 MoonAuSosiGi / moonausosigi@gmail.com
*/


// ���� ���� //////////////////////////////////////////////////////////////////////////////
HWND g_hWnd = NULL;		// ������ �ڵ�
BOOL g_bLoop = TRUE;	// ���� ���� 

char* g_ClassName = "MoonAuSosiGi SW-Renderer";					// ������ Ŭ���� ����
char* g_WindowName = "MoonAuSosiGi SWRenderer Basic Framework"; // ������ ����


//////////////////////////////////////////////////////////////////////////////////////////
BOOL InitWindow(int width, int height);							// ������ �ʱ�ȭ �Լ� 
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);	// ��������� ȣ��Ǵ� �Լ� 
BOOL MessagePump();												// �޽��� ����
LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);			// ������ ���ν���
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
//
// WinMain :: ���� �Լ�
//
int APIENTRY WinMain(HINSTANCE	hInstance,	// �ν��Ͻ�
	HINSTANCE	hPrevInstance,				// �� ���α׷� �ν��Ͻ�
	LPSTR		lpCmdLine,					// ���α׷� �ܺο��� �޾ƿ��� ����
	int		nCmdShow)						// �������α׷� ��� ���� 
{

	// ------------------------------------------- //
	// ������ ������ ����						 
	// ------------------------------------------- //
	if (!InitWindow(800, 600)) 
		return 0;

	// ------------------------------------------- //
	// Framework Setup
	// ------------------------------------------- //
	if (!MFramework::GetInstance()->SetupFramework()) 
		return 0;

	// ------------------------------------------- //
	// SW Renderer ����
	// ------------------------------------------- //

	// ------------------------------------------- //
	// ���� ����
	// ------------------------------------------- //
	while (g_bLoop)
	{
		if (!MessagePump())
			break;

	}
	// ------------------------------------------- //
	// ���ø����̼� ����
	// ------------------------------------------- //
	MFramework::GetInstance()->ReleaseFramework();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// InitWindow : ������ ���� �Լ�
//
// width : ������ ������ ���� ũ�� (pixel)
// height : ������ ������ ���� ũ�� (pixel)
//
// �Է¹��� ũ�⿡ ���� ������ ������ �����ϱ� ����
// Ŭ���̾�Ʈ ������ ���� ������ ��ü ũ�⸦ ������
//
BOOL InitWindow(int width, int height)
{
	// ---------------------------------------------------------------------//
	// ������ Ŭ���� ����ü ���
	// ---------------------------------------------------------------------//
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),CS_CLASSDC,MsgProc,0,0,
		::GetModuleHandle(NULL),NULL,NULL,
		(HBRUSH)GetStockObject(DKGRAY_BRUSH),
		NULL,g_ClassName,NULL
	};

	// ���
	RegisterClassEx(&wc);

	// ---------------------------------------------------------------------//
	// ������ ����
	// ---------------------------------------------------------------------//
	HWND hWnd = ::CreateWindow(g_ClassName, g_WindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0,
		width, height,
		GetDesktopWindow(), NULL,
		wc.hInstance, NULL
	);
	// ������ ������ �ȵǾ��� ��� 
	if (hWnd == NULL) return FALSE;

	// ������ �����ֱ�
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	// ������ �ڵ� �������� ����.
	g_hWnd = hWnd;

	//Ŭ���̾�Ʈ ���� ũ�� ������ 
	ResizeWindow(hWnd, width, height);

	return TRUE;
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
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight)
{
	// ���� �������� ��Ÿ�� ���ϱ�
	RECT oldrc;
	DWORD style = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE);

	// ���� ������ '��ü' ũ�� ( ��ũ�� ��ǥ )�� ��´�.
	::GetWindowRect(hWnd, &oldrc);

	//���� ������ �������� Ŭ���̾�Ʈ ���� ����ϱ�
	RECT newrc;
	newrc.left = 0;			newrc.top = 0;
	newrc.right = NewWidth; newrc.bottom = NewHeight;

	//newrc ��ŭ�� Ŭ���̾�Ʈ ������ �����ϴ� ������ ��ü ũ�⸦ ����
	//�޴��� ���ٴ� �����Ͽ� ó��
	//���� ����� �ٽ� newrc�� �־��ش� ( ��ũ�� ��ǥ )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, style, NULL, exstyle);

	//������ �������� �ʺ�� ���� ����
	int width = (newrc.right - newrc.left);
	int height = (newrc.bottom - newrc.top);

	// ���ο� ũ�⸦ �����쿡 ����
	::SetWindowPos(hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		width, height, SWP_SHOWWINDOW);

}

//////////////////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump() : ������ �޽��� ó�� �Լ�.
//
// ���� ���� �޽��� ó���� ���� �޽��� ������ �����Ѵ�.
// �������� OS �κ��� �� ���ø����̼����� Ư���� �޽����� ���ƿ��� �ʴ´ٸ�
// ������ ����.
//
int MessagePump()
{
	MSG msg; ::ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		//�޽��� ť�� �޽����� �ִٸ� ó��.
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) // ������ ���Ḧ ���� ����
				return FALSE;
			// ������ ó��
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else // Ư���� �޽����� ���ٸ� ������ ����
		{
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MsgProc �޽��� ���ν���
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
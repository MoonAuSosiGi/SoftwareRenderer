#include "MFramework.h"
/*
 MSWRenderer 

 main.cpp
 - WinAPI ������Ʈ ����
 
 MoonAuSosiGi / moonausosigi@gmail.com
*/

//////////////////////////////////////////////////////////////////////////////////////////
//
// WinMain :: ���� �Լ�
//
int APIENTRY WinMain(HINSTANCE	hInstance,	// �ν��Ͻ�
	HINSTANCE	hPrevInstance,				// �� ���α׷� �ν��Ͻ�
	LPSTR		lpCmdLine,					// ���α׷� �ܺο��� �޾ƿ��� ����
	int		nCmdShow)						// �������α׷� ��� ���� 
{
	MFramework* framework = MFramework::GetInstance();

	if ( framework != nullptr )
	{
		// ------------------------------------------- //
		// Framework Setup
		// ------------------------------------------- //
		if ( !framework->SetupFramework( 800, 600 ) )
			return 0;

		// ------------------------------------------- //
		// ���� ����
		// ------------------------------------------- //
		while ( framework->IsGameLoop() )
		{
			if ( !framework->MessagePump() )
				break;

		}
		// ------------------------------------------- //
		// ���ø����̼� ����
		// ------------------------------------------- //
		framework->ReleaseFramework();
	}
	

	return 0;
}
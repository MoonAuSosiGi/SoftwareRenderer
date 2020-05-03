#include "MFramework.h"
/*
 MSWRenderer 

 main.cpp
 - WinAPI 프로젝트 구성
 
 MoonAuSosiGi / moonausosigi@gmail.com
*/

//////////////////////////////////////////////////////////////////////////////////////////
//
// WinMain :: 메인 함수
//
int APIENTRY WinMain(HINSTANCE	hInstance,	// 인스턴스
	HINSTANCE	hPrevInstance,				// 전 프로그램 인스턴스
	LPSTR		lpCmdLine,					// 프로그램 외부에서 받아오는 인자
	int		nCmdShow)						// 응용프로그램 출력 형태 
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
		// 메인 루프
		// ------------------------------------------- //
		while ( framework->IsGameLoop() )
		{
			if ( !framework->MessagePump() )
				break;

		}
		// ------------------------------------------- //
		// 어플리케이션 종료
		// ------------------------------------------- //
		framework->ReleaseFramework();
	}
	

	return 0;
}
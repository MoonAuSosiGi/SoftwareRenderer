#include "MASG3MoonAu.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3VertexBuffer 정점 버퍼
// 

MASG3VertexBuffer9::MASG3VertexBuffer9( void ) 
	:	m_pVBuffer( nullptr ), 
		m_SizeInByte( 0 ), 
		m_FVF( 0 ), 
		m_Stride( 0 )
{
}

MASG3VertexBuffer9::~MASG3VertexBuffer9( void )
{
	SAFE_DELARRY( m_pVBuffer );
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 사용자 지정 옵션으로 정점버퍼 생성
//
int MASG3VertexBuffer9::Create( UINT Length,		// 정점 버퍼 전체 바이트 크기
								DWORD FVF,			// 정점 규격
								MASG3_MAPOOL Pool	// 시스템 메모리 사용
								)
{
	// 이미 만들어져있다면 에러.
	if ( MASG_VALIED( m_pVBuffer ) ) return M_FAIL;

	// POOL 옵션은 무시. 현재 SW Renderer 이므로.

	m_pVBuffer = static_cast< void* >( new BYTE[Length] );
	m_SizeInByte = Length;
	m_FVF = FVF;

	// 정점 1개의 크기 계산 
	m_Stride = 0;
	if ( CHECK( m_FVF, MASG3FVF_XY ) ) m_Stride += sizeof( float ) * 2;

	// 오류 체크
	if ( m_Stride <= 0 ) return M_FAIL;
	
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 정점 버퍼에 데이터 입력을 위한 주소 구하기.
// 전체 버퍼 중 일부분 주소 얻기도 가능.
// DX 와는 역할이 다르지만 동작 결과는 동일
//
int MASG3VertexBuffer9::Lock(	UINT OffsetToLock, // [in] 접근할 원본 버퍼의 옵셋
								void** ppbData 
							)
{
	// 유효성 검사
	if ( MASG_INVALIED( *ppbData ) ) return M_FAIL;

	// 지정 주소 계산
	BYTE* pVB = ( BYTE* ) m_pVBuffer + OffsetToLock; 
	// 결과 반환
	*ppbData = ( void* ) pVB;
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3MoonAu 개체 생성 최상위 호출 함수
//
MASG3MoonAu* MASG3MoonAuCreate9( DWORD ver )
{
	MASG3MoonAu* pMoonAu = new MASG3MoonAu;

	// 실패시 예외처리 @todo
	return pMoonAu;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 생성자와 소멸자
//
MASG3MoonAu::MASG3MoonAu( void )
{

}

MASG3MoonAu::~MASG3MoonAu( void )
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 지정 렌더링 디바이스 개체 생성 메소드
//
int MASG3MoonAu::CreateDevice(	HWND hWnd, 
								MASG3PRESENT_PARAMETERS* pp, 
								DWORD vp, 
								LPMASG3DEVICE9* ppDevice 
								)
{
	// 입력 정보 확인. @todo 오류별 리턴값 다르게 처리하도록
	if ( MASG_INVALIED( hWnd ) ) return M_FAIL;
	if ( MASG_INVALIED( pp ) ) return M_FAIL;
	if ( MASG_INVALIED( ppDevice ) ) return M_FAIL;

	// 디바이스 개체 생성 -> DX9 같이만 처리하자
	LPMASG3DEVICE9 pDev = new MASG3MoonAuDevice9;

	// 초기화
	pDev->m_hWnd = hWnd;
	pDev->m_PresentParam = *pp;
	pDev->m_VertexProcessing = vp;
	pDev->_CreateRenderTarget();

	// 외부 반환
	*ppDevice = pDev;

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3MoonAuDevice9 : DX 9 대응 디바이스 클래스
//
// 실제 렌더링 관련 정보를 처리하는 클래스
//

MASG3MoonAuDevice9::MASG3MoonAuDevice9( void )
{
	m_hWnd = nullptr;
	ZeroMemory( &m_PresentParam, sizeof( m_PresentParam ) );

	m_hBmpRT = nullptr;
	m_hSurfaceRT = nullptr;
	m_BkColor = RGB( 0, 0, 255 );

	m_pVB = nullptr;
	m_FVF = 0;
	m_Stride = 0;
	m_PrimCnt = 0;
	m_StartVtx = 0;
}

MASG3MoonAuDevice9::~MASG3MoonAuDevice9( void )
{
	_ReleaseRenderTarget();

	// 정점 버퍼는 사용자가 책임을 진다 (제거를 맡김)
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
HDC MASG3MoonAuDevice9::GetRT()
{
	return m_hSurfaceRT;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
COLORREF MASG3MoonAuDevice9::GetBkColor()
{
	return m_BkColor;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 백버퍼용 렌더타겟 생성
//
int MASG3MoonAuDevice9::_CreateRenderTarget()
{
	if ( MASG_INVALIED( m_hWnd ) ) return M_FAIL;

	HDC hdc = GetDC( m_hWnd );
	m_hSurfaceRT = CreateCompatibleDC( hdc );
	m_hBmpRT = ( HBITMAP ) CreateCompatibleBitmap( hdc, m_PresentParam.Width, m_PresentParam.Height );
	SelectObject( m_hSurfaceRT, m_hBmpRT );

	ReleaseDC( m_hWnd, hdc );

	return M_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// 
// 렌더타겟 제거
//
void MASG3MoonAuDevice9::_ReleaseRenderTarget()
{
	DeleteObject( m_hBmpRT );
	DeleteDC( m_hSurfaceRT );
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 장면 그리기 시작 : 렌더링에 필요한 선 작업을 수행
//
int MASG3MoonAuDevice9::BeginScene()
{
	// 폰트 배경색은 투명을 허용
	SetBkMode( m_hSurfaceRT, TRANSPARENT );

	// 기본 펜 색상
	HPEN hPen = ( HPEN ) GetStockObject( WHITE_PEN );
	SelectObject( m_hSurfaceRT, hPen );

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 장면 그리기 종료 : 렌더링 끝난 후 작업을 수행
//
int MASG3MoonAuDevice9::EndScene()
{
	// ...
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 렌더타겟 클리어 (배경 클리어)
//
int MASG3MoonAuDevice9::ClearColor( COLORREF color )
{
	HBRUSH hBrush = CreateSolidBrush( color );
	RECT rc = { 0, 0, static_cast< long >( m_PresentParam.Width ), static_cast< long >( m_PresentParam.Height ) };
	FillRect( m_hSurfaceRT, &rc, hBrush );
	DeleteObject( hBrush );

	m_BkColor = color;

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 렌더타겟의 내용을 FrontBuffer에 출력  (Flipping, Swapping)
//
int MASG3MoonAuDevice9::Present()
{
	HDC hdc = GetDC( m_hWnd );
	BitBlt( hdc, 0, 0, m_PresentParam.Width, m_PresentParam.Height, m_hSurfaceRT, 0, 0, SRCCOPY );
	ReleaseDC( m_hWnd, hdc );

	return M_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
// 사용자 지정 옵션으로 정점 버퍼 생성
//
int MASG3MoonAuDevice9::CreateVertexBuffer( UINT Length, 
											DWORD FVF, 
											MASG3_MAPOOL Pool, 
											LPMASG3VERTEXBUFFER9* ppVB 
											)
{
	LPMASG3VERTEXBUFFER9 pVB = new MASG3VertexBuffer9;

	if ( MASG_INVALIED( pVB ) ) return M_FAIL;

	if ( MASG_FAILED( pVB->Create( Length, FVF, Pool ) ) )
	{
		// 에러처리
		return M_FAIL;
	}

	// 외부 반환
	*ppVB = pVB;

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 사용자 생성 정점버퍼를 디바이스에 등록
//
int MASG3MoonAuDevice9::SetVertexBuffer( LPMASG3VERTEXBUFFER9 pVB, UINT Stride )
{
	if ( MASG_INVALIED( pVB ) ) return M_FAIL;

	m_pVB = pVB;
	m_Stride = Stride;

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 정점 규격을 전달
// SetStreamSource 로 등록하거나, 등록할 정점 버퍼와 동일한 규격이어야 함
//
int MASG3MoonAuDevice9::SetFVF( DWORD fvf )
{
	m_FVF = fvf;
	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 기하 데이터 렌더링!
// 정상 동작을 위해선 정점버퍼 , FVF, 마디 크기 등 모든 정보가 등록되어있어야 함.
//
int MASG3MoonAuDevice9::DrawPrimitive(	MASG3PRIMITIVETYPE PrimitiveType,	// 렌더링 기하 타입
										UINT StartVertex,					// 렌더링할 정점 시작번호 (정점버퍼 안에서)
										UINT PrimitiveCount					// 렌더링할 기하 갯수
										)
{
	if ( MASG_INVALIED( m_pVB ) ) return M_FAIL;
	if ( m_Stride == 0 ) return M_FAIL;

	// 기초 옵션 저장
	m_PrimCnt = PrimitiveCount;
	m_StartVtx = StartVertex;

	// 렌더링 파이프라인 시작
	// 1. 정점 파이프라인
	_VertexPipeLine();
	// 2. 기하 파이프라인
	_GeometryPipeLine();
	// 3. 픽셀 파이프라인
	_PixelPipeLine();

	// 4. 최종 출력

	return M_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// _VertexPipeLine
// 버텍스 파이프라인 -> 정점 연산을 수행하고, 결과는 GP 로 넘어간다.
//
int MASG3MoonAuDevice9::_VertexPipeLine()
{
	return M_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 
// _GeoMetryPipeLine 
// 지오메트리 파이프라인 -> 기하 연산을 수행하고, 결과는 PP 로 넘어간다.
//
int MASG3MoonAuDevice9::_GeometryPipeLine()
{
	return M_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// _PixelPipeLine
// 픽셀 파이프라인 -> 픽셀당 연산을 수행하고, 결과는 RT에 기록된다.
//
int MASG3MoonAuDevice9::_PixelPipeLine()
{
	// 정점 갯수 확인
	UINT vtxCnt = m_pVB->m_SizeInByte / m_Stride;

	// 원본 버퍼 얻기
	void* pOrgVB = m_pVB->_GetBuffer();

	BYTE* pCurrVB = ( BYTE* ) pOrgVB;	// 현재 처리중인 정점버퍼의 주소
	BYTE* pCurrVtx = nullptr;			// 현재 처리중인 정점 주소
	UINT faceCnt = 0;

	// Triangle List를 먼저 처리. -> Face 마다 3개의 정점 사용
	for ( UINT i = m_StartVtx; i < vtxCnt; i += 3 )
	{
		// 현재 기하 데이터 포인터 얻기
		pCurrVB = ( BYTE* ) pOrgVB + ( i ) * m_Stride;

		// 첫번째 정점
		pCurrVtx = pCurrVB;
		MASG3VECTOR2 v0 = *( MASG3VECTOR2* ) pCurrVtx;

		// 두번째 정점
		pCurrVtx = pCurrVB + m_Stride;
		MASG3VECTOR2 v1 = *( MASG3VECTOR2* ) pCurrVtx;

		// 세번째 정점
		pCurrVtx = pCurrVB + m_Stride * 2;
		MASG3VECTOR2 v2 = *( MASG3VECTOR2* ) pCurrVtx;

		// 라인 그리기
		// v0 - v1
		_DrawLine( v0, v1 );
		// v0 - v2
		_DrawLine( v0, v2 );
		// v1 - v2
		_DrawLine( v1, v2 );

		// 지정 갯수 이상의 삼각형이 그려지면 끝
		if ( ++faceCnt >= m_PrimCnt )
			break;
	}

	return M_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// DrawLine 
//
int MASG3MoonAuDevice9::_DrawLine( MASG3VECTOR2& start, MASG3VECTOR2& end )
{
	// GDI 작업 
	// @todo 직접 라인 그리기

	// 직선의 방정식
	// 1 y = mx + b   
	//    m : 기울기  dy / dx 실수
	// 2 ax + by + c = 0  
	//   y = - (a/b)x  -(c/b) 마찬가지로 실수

	// 실제의 직선은 실수로 이어진 직선이지만,
	// 컴퓨터 화면은 픽셀로 되어 있어 정수로 표현된다. 
	// 따라서, 실수 계산을 하더라도, 정수로 표현해야함. 
	
	// DDA 알고리즘
	// * 실수 곱이 필요 없음
	// * 실수 덧셈은 있음

	// 브레슨햄 알고리즘
	// * 실수 연산이 필요 없음
	// * 오로지 정수 연산
	// xi , yi 를 지정하고, 직선의 방정식과 가장 가까운 정수 좌표에 픽셀을 찍음

	// y = mx +b
	// yi +1    d2
	// y
	// yi -1    d1   d1이 크면, yi+1 에 찍고, d2가 크면 yi-1에 찍는다.
	// d1 - d2 

	int stX = ( int ) start.x;
	int stY = ( int ) start.y;
	int edX = ( int ) end.x;
	int edY = ( int ) end.y;
	// x, y 가 정해졌다고 가정 (stX, stY) 
	// y = mx+b 
	int dx = abs((int)(end.x - start.x));
	int dy = abs((int)(end.y - start.y));

	COLORREF color = RGB( 255, 255, 255 );

	int inc_2dy = 2 * dy;
	int inc_2dydx = 2 * ( dy - dx );
	int ndx = 0;
	int x1 = start.x, x2 = end.x;
	int y1 = start.y, y2 = end.y;
	int inc_value = 1;
	int p_value = 0;

	// x의 변화량이 더 클떄 ( y값을 정해주어야 함 )
	if ( dx >= dy )
	{
		if ( end.x < start.x )
		{
			ndx = x1;
			x1 = x2;
			x2 = ndx;

			ndx = y1;
			y1 = y2;
			y2 = ndx;
		}

		if ( y1 < y2 )  inc_value = 1;
		else			inc_value = -1;

		SetPixel( m_hSurfaceRT, x1, y1, color );
		p_value = 2 * dy - dx;

		for ( int i = x1; i < x2; i++ )
		{
			if ( 0 > p_value )
				p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				y1 += inc_value;
			}
			SetPixel( m_hSurfaceRT, i, y1, color );
		}
	}
	else
	{
		inc_2dy = 2 * dx;
		inc_2dydx = 2 * ( dx - dy );

		if ( y2 < y1 )
		{
			ndx = y1;
			y1 = y2;
			y2 = ndx;

			ndx = x1;
			x1 = x2;
			x2 = ndx;
		}

		if ( x1 < x2 )	inc_value = 1;
		else			inc_value = -1;

		SetPixel( m_hSurfaceRT, x1, y1, color );

		p_value = 2 * dx - dy;

		for ( int i = y1; i < y2; i++ )
		{
			if ( 0 > p_value )
				p_value += inc_2dy;
			else
			{
				p_value += inc_2dydx;
				x1 += inc_value;
			}
			SetPixel( m_hSurfaceRT, x1, i, color );
		}

	}
	

	//MoveToEx( m_hSurfaceRT, ( int ) start.x, ( int ) start.y, nullptr );
	//LineTo( m_hSurfaceRT, ( int ) end.x, ( int ) end.y );

	return M_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// 렌더링 상태 옵션 변경
//
int MASG3MoonAuDevice9::SetRenderState( DWORD state, DWORD value )
{
	return M_SUCCESS;
}
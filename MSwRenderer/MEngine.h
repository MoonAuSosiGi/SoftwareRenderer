#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////
//
#define MASG_INVALIED(res) ((res) == nullptr)
#define MASG_VALIED(res) ((res) != nullptr)
#define MASG_FAILED(res) ((res) < 0)

/////////////////////////////////////////////////////////////////////////////////////////////
//
#include <assert.h>
#define ASSERT(Val) assert((Val))

#define M_FAIL			-1
#define M_SUCCESS		0


/////////////////////////////////////////////////////////////////////////////////////////////
//
// 렌더 타겟 (백버퍼) 설정 구조체 : D3DPRESENT_PARAMETERS 대응
//
struct MASG3PRESENT_PARAMETERS 
{
	DWORD Width;			// 클라이언트 영역 기준 해상도
	DWORD Height;
	DWORD BackBuffercnt;	// 백버퍼 갯수
	BOOL Windowed;			// 창모드 여부
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 해상도 정보 구조체 : D3DISPLAYMODE 대응
//
struct MASG3DISPLAYMODE
{
	DWORD Width, Height;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// 정점 처리 - 가속화 정의
//
enum MASG3_BEHAVIOR_FLAG
{
	MASG3_CREATE_SOFTWARE_VERTEXPROCESSING, // CPU 정점 연산 처리
	MASG3_CREATE_HARDWARE_VERTEXPROCESSING, // GPU 정점 연산 처리 
};

// 메모리 관리 영역 옵션
enum MASG3_MAPOOL
{
	MASG3POOL_DEFAULT = 1,	// 기본값 : 비디오 메모리 사용
	MASG3POOL_MANAGED,		// 자동관리 : 비디오 - 시스템 혼용
	MAS3GPOOL_SYSTEM			// 시스템 메모리 사용 ( 이 옵션만 유효하다 현재 )
};

// 기하 도형 타입
enum MASG3PRIMITIVETYPE
{
	MASG3_POINTLIST = 1,
	MASG3_LINELIST,
	MASG3_LINESTRIP,
	MASG3_TRIANGLELIST,
	MASG3_TRIANGLESTRIP,
	MASG3_TRIANGLEFAN
};

// FVF 옵션 - 정점 규격 정의
#define MASG3FVF_XY				0x0001			// 2D 좌표 (DX에는 없음)
#define MASG3FVF_XYZ				0x0002			// 3D 좌표 (미변환) local
#define MASG3FVF_XYZRHW			0x0004			// 3D 좌표 (변환 완료) XY + Depth
#define MASG3FVF_NORMAL			0x0010			// 노멀
#define MASG3FVF_DIFFUSE			0x0040			// 확산색(Diffuse) 색상
#define MASG3FVF_SPECULAR		0x0080			// 정반사(Specular) 색상

#define CHECK( value, bit ) (((value) & (bit)) == bit)

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

class MASG3MoonAuDevice9;
typedef MASG3MoonAuDevice9	MASG3MOONAUDEVICE9;	// DX 9
typedef MASG3MOONAUDEVICE9*	LPMASG3DEVICE9;

#define MASG_VERSION	9

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3VertexBuffer
// 정점 버퍼 관리 클래스. Dx9의 IDirect3DVertexBuffer9과 대응된다.
//
/////////////////////////////////////////////////////////////////////////////////////////////
class MASG3VertexBuffer9
{
	friend class MASG3Device9;
protected:
	void* m_pVBuffer;		// 정점 버퍼 : 실제 데이터가 저장됨
	
	DWORD m_SizeInByte;		// 정점 버퍼 크기 (byte)
	DWORD m_FVF;			// 정점 버퍼 규격 조합 플래그
	UINT m_Stride;			// 정점 버퍼 1마디 (정점 구조 하나)의 크기

protected:
	MASG3VertexBuffer9( void );	// 객체 생성 방지로 숨김
	
	void* _GetBuffer() { return m_pVBuffer; }
public:
	virtual ~MASG3VertexBuffer9( void );

	// 정점 버퍼 생성 : DX - 주어진 옵션에 맞추어 VRAM 을 확보
	int Create( UINT Length,		// 정점 버퍼의 전체 크기 (Byte)
				DWORD FVF,			// 정점 규격
				MASG3_MAPOOL Pool	// 메모리 사용 옵션
			);

	// 정점 버퍼 주소 얻기 -> 실제 DX의 그것과는 다르지만 결과는 동일함.
	int Lock( UINT OffsetToLock,	// [in] 접근할 원본 버퍼의 오프셋 주소. 기본값은 0
			  void** ppbData		// [out] 리턴 받을 포인터
		);
	
	// 정점 버퍼 닫기 (딱히 하는일 없음)
	void Unlock() {}
};

typedef MASG3VertexBuffer9* LPMASG3VERTEXBUFFER9;

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3MoonAu SWRenderer : 렌더링 디바이스들을 관리할 최상위 클래스
//					  DirectX 9과 흡사하게 구현하는 것이 목표
//
/////////////////////////////////////////////////////////////////////////////////////////////

class MASG3MoonAu
{
	friend MASG3MoonAu* MASG3MoonAuCreate9( DWORD ver );

protected:
	MASG3MoonAu( void );	// 객체 생성 방지
public:
	virtual ~MASG3MoonAu( void );

	// 외부 노출 메서드 -> DX 와 동일하게 구현하는 것이 목표
	int CreateDevice( HWND hWnd,						// [in] 디바이스의 렌더링 목표 윈도우 핸들
					  MASG3PRESENT_PARAMETERS* pp,		// [in] 디바이스 화면 구성 정보
					  DWORD vp,							// [in] 정점 연산 방법 결정 (현재는 SW 만)
					  LPMASG3DEVICE9* pDev				// [out] 성공시 리턴받을 디바이스 개체 포인터
		);
};

typedef MASG3MoonAu* LPMASG3MOONAU;

// MASG3MoonAu9 개체 생성 함수
MASG3MoonAu* MASG3MoonAuCreate9( DWORD ver );

/////////////////////////////////////////////////////////////////////////////////////////////
//
// MASG3MoonAuDevice9 : DX9에 대응되는 렌더링 디바이스 클래스
//						IDirect3DDevice9에 대응
//
//						렌더링 관련 정보를 처리
//
class MASG3MoonAuDevice9
{
	friend class MASG3MoonAu;
protected:

	// 출력 화면 관련 정보
	HWND						m_hWnd;
	MASG3PRESENT_PARAMETERS		m_PresentParam;
	DWORD						m_VertexProcessing;
	MASG3DISPLAYMODE			m_Display;

	// 렌더 타겟 (Back-Buffer) 구성용 핸들
	HBITMAP						m_hBmpRT;
	HDC							m_hSurfaceRT;
	COLORREF					m_BkColor;

	// 정점 버퍼
	LPMASG3VERTEXBUFFER9		m_pVB;		// 렌더링용 등록된 정점 버퍼
	DWORD						m_FVF;		// 렌더링용 등록된 정점 규격
	UINT						m_Stride;	// 렌더링용 등록된 정점 1마디 크기

	// 그리기 옵션 -> Draw Primitive 호출시 입력됨
	UINT						m_PrimCnt;		// 이번에 그려질 Face 갯수
	UINT						m_StartVtx;	// 이번에 그려질 시작 정점 번호

protected:
	
	// 렌더 타겟 생성
	int _CreateRenderTarget();
	// 렌더 타겟 제거
	void _ReleaseRenderTarget();

	// 렌더링 파이프라인 메소드
	int _VertexPipeLine();
	int _GeometryPipeLine();
	int _PixelPipeLine();

};
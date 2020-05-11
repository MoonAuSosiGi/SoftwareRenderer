#pragma once

#include <windows.h>

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
// DXUtil.h 참고
//

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = nullptr; }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = nullptr; }
#endif
#ifdef SAFE_DELETE_ARRAY
#define SAFE_DELARRY  SAFE_DELETE_ARRAY
#else
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = nullptr; }
#endif
#pragma once

#define MASG_PI				3.141592f
#define MASG_ToRadian(a)	(a / 180.0f * MASG_PI)
#define MASG_ToAngle(r)		(r * 180.0f / MASG_PI)
#define MASG_MakeRadian(do) ((float)(MASG_PI / 180.0f) * (float)(do))
#define MASG_ToDegree(r)	MASG_ToAngle(r)

/////////////////////////////////////////////////////////////////////////////////////////////
// 
// Vector 
// 수직:열 기준 column major
//

// 2성분 벡터
struct MASG3VECTOR2
{
	float x;
	float y;
};

// 3성분 벡터
struct MASG3VECTOR3
{
	float x;
	float y;
	float z;
};

// 4성분 벡터
struct MASG3VECTOR4
{
	float x;
	float y;
	float z;
	float w;
};

// Vector2 
class MASG3XVECTOR2
{
public:
	float x, y;
public:
	MASG3XVECTOR2( float _x, float _y ) { x = _x; y = _y; }
	MASG3XVECTOR2( MASG3VECTOR2 v ) { x = v.x; y = v.y; }
};
typedef MASG3VECTOR2 VECTOR2;

// Vector3
class MASG3XVECTOR3
{
public:
	float x, y, z;
public:
	MASG3XVECTOR3( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; }
	MASG3XVECTOR3( MASG3VECTOR3 v ) { x = v.x; y = v.y; z = v.z; }
};
typedef MASG3XVECTOR3 VECTOR3;

// Vector4
class MASG3XVECTOR4
{
public:
	float x, y, z, w;
public:
	MASG3XVECTOR4( float _x, float _y, float _z, float _w ) { x = _x; y = _y; z = _z; }
	MASG3XVECTOR4( MASG3VECTOR4 v ) { x = v.x; y = v.y; z = v.z; }
};
typedef MASG3XVECTOR4 VECTOR4;

// 컬러
struct MASG3COLOR4 
{
	float r, g, b, a;
};

typedef MASG3COLOR4 COLOR;

#define TOCOLOR(col) (*(COLOR*)&(col))
COLOR MASG3XColor( float r, float g, float b, float a );
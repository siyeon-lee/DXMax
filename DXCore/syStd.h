#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <memory>
#include <fstream>
#include <iostream>
#include "syDxHelperEx.h"

using namespace Microsoft::WRL;
using namespace std;
using namespace DX;

#if defined(DEBUG) || defined(_DEBUG)
	#pragma comment( lib, "DXCore64_D.lib" )
#else
	#pragma comment( lib, "DXCore64_R.lib" )
#endif

#pragma comment( lib, "msimg32")

//WinMain 메크로
#define syCORE_START int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR    lpCmdLine,_In_ int       nCmdShow){
//#define syCORE_MAIN(s,x,y,w,h) sample  sample; sample.InitWindow(hInstance, L#s,x,y,w,h); sample.InitDevice(); sample.Run();
#define syCORE_MAIN(s,x,y,w,h) sample  sample; sample.InitWindow(hInstance, L#s,x,y,w,h); sample.Run();
#define syCORE_END }

#define syCORE_RUN(s,x,y,w,h) syCORE_START; syCORE_MAIN(s,x,y,w,h); syCORE_END;

#define randstep(fMin,fMax) (fMin+((float)fMax-(float)fMin)*rand()/(float)RAND_MAX)

struct syPoint
{
	float  x;
	float  y;
	syPoint() {};
	syPoint(float fx, float fy) { x = fx; y = fy; }
};
struct syRect
{
	float    left;
	float    top;
	float    right;
	float    bottom;
	syRect() {};
	syRect(float a, float b, float c, float d) {
		left = a;
		top = b;
		right = c;
		bottom = d;
	};
};
struct SY_BOX
{
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vAxis[3];
	float       fExtent[3];
	/*SY_BOX(D3DXVECTOR3 min, D3DXVECTOR3 max) {
		vMin = min;
		vMax = max;
		vCenter = (min + max)/ 2;
		vAxis[0] = D3DXVECTOR3(max.x - vCenter.x, vCenter.y, vCenter.z);
		D3DXVec3Normalize(&vAxis[0], &vAxis[0]);
	};*/
};
struct SY_SPHERE
{
	D3DXVECTOR3 vCenter;
	float       fRadius;
};
struct sySphere
{
	syPoint vCenter;
	float fRadius;
};

struct syVertex
{
	syPoint   p; // client pos
	syPoint   t; // bitmap pos
	syVertex() {};
	syVertex(syPoint a, syPoint b)
	{
		p = a;
		t = b;
	};
	syVertex(float px, float py, float tx, float ty)
	{
		p.x = px; p.y = py;
		t.x = tx; t.y = ty;
	};
};

struct syIndex
{
	int x;
	int y;
	bool operator == (const syIndex& p)
	{
		return (x == p.x && y == p.y);
	}
	syIndex operator + (const syIndex& b)
	{
		return { x + b.x,y + b.y };
	}
	syIndex() { x = y = 0; }
	syIndex(int fx, int fy) { x = fx; y = fy; }
};
using CoordinateList = std::vector<syIndex>;

//유니코드<->멀티바이트 코드 관련
typedef std::basic_string<TCHAR> T_STR;		//tchar
typedef std::basic_string<char> C_STR;		//std::string
typedef std::basic_string<WCHAR> W_STR;		//std::wstring

static std::wstring mtw(std::string str)
{
	std::wstring ret = std::wstring(str.begin(), str.end());
	return ret;
}

static std::string wtm(std::wstring str)
{
	std::string ret = std::string(str.begin(), str.end());
	return ret;
}

static char* GetWtM(WCHAR* src)
{
	char retData[1024] = { 0, };
	int iLength = WideCharToMultiByte(CP_ACP, 0, 
		src, -1, 0, 0, NULL, NULL);	//길이 받아옴(리턴값이 변환에 필요한 길이)

	int iRet = WideCharToMultiByte(CP_ACP, 0,
		src, -1, retData, iLength, NULL, NULL);

	if (iRet == 0) return nullptr;
	return retData;
}
static bool  GetWtM(WCHAR* src, char* pDest)
{
	int iLength = WideCharToMultiByte(CP_ACP, 0,
		src, -1, 0, 0, NULL, NULL);

	int iRet = WideCharToMultiByte(CP_ACP, 0,
		src, -1, pDest, iLength, NULL, NULL);
	if (iRet == 0) return false;
	return true;
}



static WCHAR*  GetMtW(CHAR* src)
{
	WCHAR retData[1024] = { 0, };
	int iLength = MultiByteToWideChar(CP_ACP, 0, src, -1, 0, 0);

	int iRet = MultiByteToWideChar(CP_ACP, 0,
		src, -1, retData, iLength);
	if (iRet == 0) return nullptr;
	return retData;
}
static bool  GetMtW(CHAR* src, WCHAR* pDest)
{
	int iLength = MultiByteToWideChar(CP_ACP, 0,
		src, -1, 0, 0);
	int iRet = MultiByteToWideChar(CP_ACP, 0,
		src, -1, pDest, iLength);
	if (iRet == 0) return false;
	return true;
}



struct TInputActionMap
{
	DWORD	bENTERKEY;
	DWORD   bUPKEY;
	DWORD   bDOWNKEY;
	DWORD   bLEFTKEY;
	DWORD   bRIGHTKEY;
	DWORD   bLeftClick;
	DWORD   bRightClick;
	DWORD   bMiddleClick;
	DWORD   bWheel;
	DWORD   bExit;
	DWORD   bSpace; // left mouse, A key
};
enum { KEY_FREE, KEY_PUSH, KEY_HOLD, KEY_UP };
enum {WHEEL_UP, WHEEL_DOWN, WHEEL_FREE};
extern TCHAR g_username[];
extern ID3D11Device* g_pDevice;
extern int g_score;
extern TInputActionMap  g_ActionInput;
extern HWND  g_hWnd;
extern HINSTANCE g_hInstance;
extern POINT g_MousePos;
extern float g_fGameTimer;
extern float g_fSecondPerFrame;
extern RECT  g_rtClient;
template <class T>
class sySingleton
{
public:
	static T& GetInstance()
	{
		static T singleton;
		return singleton;
	}
};

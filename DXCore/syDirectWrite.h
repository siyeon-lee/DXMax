#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <DWrite.h>
#include <string>
#include <vector>
#pragma comment( lib, "d2d1.lib")
#pragma comment( lib, "dwrite.lib")
struct syTextArray
{
	D2D1_MATRIX_3X2_F matWorld;
	D2D1_RECT_F rf;
	D3DCOLORVALUE color;
	std::wstring data;
	IDWriteTextFormat* pTextFormat;
	syTextArray()
	{
		matWorld = D2D1::IdentityMatrix();
		color = D2D1::ColorF(0, 0, 0, 1);
		rf.top = 0;
		rf.left = 0;
		rf.right = 800;
		rf.bottom = 600;
		pTextFormat = nullptr;
	}
	syTextArray(RECT rt)
	{
		matWorld = D2D1::IdentityMatrix();
		color = D2D1::ColorF(0, 0, 0, 1);
		rf.top = rt.top;
		rf.left = rt.left;
		rf.right = rt.right;
		rf.bottom = rt.bottom;
		pTextFormat = nullptr;
	}
};
class syDirectWrite
{
public:
	syDirectWrite();
	virtual ~syDirectWrite();
public:
	std::vector<syTextArray> m_dataList;
private:
	float m_fdpiX;
	float m_fdpiY;
	float m_fdpiScaleX;
	float m_fdpiScaleY;
public:
	//IndependentResources
	ID2D1Factory*		 m_pd2dFactory;
	IDWriteFactory*      m_pDWriteFactory;
	IDWriteTextFormat*   m_pTextFormat;
	//DeviceResource
	ID2D1RenderTarget*   m_pRT;
	ID2D1SolidColorBrush* m_pBlackBrush;

	IDWriteTextLayout*   m_pTextLayout;
public:
	bool       Init();
	bool       Frame();
	bool       Render();
	bool       Release();
public:
	void   Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1*  pSurface);
	HRESULT CreateDeviceIndependentResources();
	void	DiscardDeviceIndependentResources();
	HRESULT CreateDeviceResources(IDXGISurface1* pSurface);
	void	DiscardDeviceResource();
};


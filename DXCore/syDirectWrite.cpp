#include "syDirectWrite.h"



syDirectWrite::syDirectWrite()
{
}


syDirectWrite::~syDirectWrite()
{
}
bool       syDirectWrite::Init()
{
	return true;
}
bool       syDirectWrite::Frame()
{
	return true;
}
bool       syDirectWrite::Render()
{
	m_pRT->BeginDraw();
	for (int iData = 0; iData < m_dataList.size(); iData++)
	{
		m_pRT->SetTransform(m_dataList[iData].matWorld);
		m_pBlackBrush->SetColor(m_dataList[iData].color);

		IDWriteTextFormat*   pTextFormat = m_dataList[iData].pTextFormat;
		if (pTextFormat == nullptr)
		{
			pTextFormat = m_pTextFormat;
		}
		m_pRT->DrawText(m_dataList[iData].data.c_str(),
			m_dataList[iData].data.size(),
			pTextFormat, m_dataList[iData].rf,
			m_pBlackBrush);
	}
	m_pRT->EndDraw();
	return true;
}
bool       syDirectWrite::Release()
{
	DiscardDeviceResource();
	DiscardDeviceIndependentResources();
	return true;
}

void   syDirectWrite::Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1*  pSurface)
{
	CreateDeviceIndependentResources();
	CreateDeviceResources(pSurface);
}
HRESULT syDirectWrite::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pd2dFactory);
	m_pd2dFactory->GetDesktopDpi(&m_fdpiX, &m_fdpiY);
	m_fdpiScaleX = m_fdpiX / 96.0f;
	m_fdpiScaleY = m_fdpiY / 96.0f;		//96dpi가 많이 보이는데, 1920×1080 해상도의 23인치 모니터에 해당하는 수치이다.


	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pDWriteFactory);
	hr = m_pDWriteFactory->CreateTextFormat(
		L"Gabriola",
		//L"궁서", 
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20,
		//L"ko-kr", 
		L"en-us",
		&m_pTextFormat);
	return hr;
}
void	syDirectWrite::DiscardDeviceIndependentResources()
{
	if (m_pTextFormat) m_pTextFormat->Release();
	if (m_pDWriteFactory) m_pDWriteFactory->Release();
	if (m_pd2dFactory) m_pd2dFactory->Release();
}
HRESULT syDirectWrite::CreateDeviceResources(IDXGISurface1* pSurface)
{
	HRESULT hr = S_OK;

	D2D1_RENDER_TARGET_PROPERTIES rp;
	rp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,D2D1_ALPHA_MODE_PREMULTIPLIED);
	rp.dpiX = m_fdpiX;
	rp.dpiY = m_fdpiY;
	rp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	rp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

	if (FAILED(hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pSurface, &rp, &m_pRT)))
	{
		return hr;
	}
	if (FAILED(hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush)))
	{
		return hr;
	}
	return hr;

}
void	syDirectWrite::DiscardDeviceResource()
{
	if (m_pRT) m_pRT->Release();
	if (m_pBlackBrush) m_pBlackBrush->Release();
}